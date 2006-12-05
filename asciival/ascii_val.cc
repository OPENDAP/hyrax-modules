
// -*- mode: c++; c-basic-offset:4 -*-

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
// more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1998-2000
// Please read the full copyright statement in the file COPYRIGHT_URI.  
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

/** Asciival is a simple filter program similar to geturl or writeval that
    reads a data object from a DAP server (either by dereferencing a URL,
    reading from a file or reading from stdin) and writes comma separated
    ASCII representation of the data values in that object.

    Past versions could read from several URLs. This version can read a
    single URL _or_ can read from a local file using a handler (run in a sub
    process). It cannot read from several URLs. That feature was never used
    and was hard to implement with the new file/handler mode. The
    file/handler mode makes server operation much more efficient since the
    data are not read by making another trip to the server. Instead, the
    handler, running in a subprocess, will return data directly to asciival.
    
    The -r 'per url' option has been removed. This was also not ever used.

    @todo The code in read_from_file should really be moved into DDS. Or
    DataDDS. That is, DataDDS should have the ability to read and parse not
    only the DDS part of a 'DODS' response, but also load those variables with
    data. It should also be smart enough to sort out Error objects and throw
    them, something the code here (and also in Connect::process_data()) does
    not do.

    @author: jhrg */

#include "config.h"

static char rcsid[] not_used =
    { "$Id$" };

#include <stdio.h>

#include <string>
#include <iostream>

using namespace std;

#include <GetOpt.h>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <BaseType.h>
#include <Connect.h>
#include <PipeResponse.h>
#include <cgi_util.h>
#include <debug.h>

#include "AsciiByte.h"
#include "AsciiInt32.h"
#include "AsciiUInt32.h"
#include "AsciiFloat64.h"
#include "AsciiStr.h"
#include "AsciiUrl.h"
#include "AsciiArray.h"
#include "AsciiStructure.h"
#include "AsciiSequence.h"
#include "AsciiGrid.h"

#include "AsciiOutputFactory.h"

#include "get_ascii.h"

using namespace dap_asciival;

static void usage()
{
    cerr << "Usage: \n"
        << " [mnhwvruVt] -- [<url> | <file> ]\n"
        << "       m: Output a MIME header.\n"
        <<
        "       f <handler pathname>: Use a local handler instead of reading from a URL\n"
        << "          This assumes a local file, not a url.\n" <<
        "       w: Verbose (wordy) output.\n" <<
        "       v <string>: Read the server version information.\n" <<
        "       u <url>: URL to the referenced data source.\n" <<
        "       r <dir>: Path to the cache directory.\n" <<
        "       e <expr>: Constraint expression.\n" <<
        "       V: Print the version number and exit.\n" <<
        "       h: Print this information.\n" <<
        "<url> may be a true URL, which asciival will dereference,\n" <<
        "it may be a local file or it may be standard input.\n" <<
        "In the later case use `-' for <url>.\n";
}

static void
read_from_url(DataDDS & dds, const string & url, const string & expr)
{
    Connect *c = new Connect(url);

    c->set_cache_enabled(false);        // Server components should not cache...

    if (c->is_local()) {
        string msg = "Error: URL `";
        msg += string(url) + "' is local.\n";
        throw Error(msg);
    }

    c->request_data(dds, expr);

    delete c;
    c = 0;
}

static void
read_from_file(DataDDS & dds, const string & handler,
               const string & options, const string & file,
               const string & expr)
{
    string command = handler + " -o dods " + options
        + " -e " + "\"" + expr + "\"" + " \"" + file + "\"";

    DBG(cerr << "DDS Command: " << command << endl);

    FILE *in = popen(command.c_str(), "r");
    PipeResponse pr(in);
#if 1
    // Cheat: Instead of using our own code, use code from Connect. Really we should
    // make a FILEConnect (just like there's an HTTPConnnect) to process inputs
    // from files. For now, use the read_data() method. jhrg 2/9/06
    Connect c("local_pipe");
    c.read_data(dds, &pr);
#else
    if (in && remove_mime_header(in)) {
        XDR *xdr_stream = 0;

        try {
            dds.parse(in);
            xdr_stream = new_xdrstdio(in, XDR_DECODE);

            // Load the DDS with data.
            DDS::Vars_iter i;
            for (i = dds.var_begin(); i != dds.var_end(); i++) {
                (*i)->deserialize(xdr_stream, &dds);
            }
        }
        catch(Error & e) {
            if (xdr_stream)
                delete_xdrstdio(xdr_stream);
            throw e;
        }

        delete_xdrstdio(xdr_stream);

        pclose(in);
    }
#endif
}

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out (since that will confuse loaddods).

    @author jhrg */

static void output_error_object(const Error & e)
{
    if (e.OK())
        cout << "Error: " << e.get_error_message() << endl;
}

/*
 *     } elsif ( $self->ext() eq "ascii" || $self->ext() eq "asc" ) {
        $options    = "-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir();
        }

        @command = ( $self->asciival(), $options, "-m", 
                     "-u", $self->url_text(),
                     "-f", $self->handler(), 
                     "--", $self->filename() ); #. "?" . $self->query() );
 */

// Read a DODS data object. The object maybe specified by a URL (which will
// be dereferenceed using Connect, it maybe read from a file or it maybe read
// from stdin. Use `-' in the command line to indicate the next input should
// be read from standard input.

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "nf:mv:r:u:e:wVh?");
    int option_char;
    bool verbose = false;
    bool handler = false;
    bool mime_header = false;
    bool version = false;
    bool cache = false;
    bool url_given = false;
    bool expr_given = false;

    string expr = "";
    string handler_name = "";
    string server_version = "";
    string cache_dir = "/usr/tmp";
    string url = "";
    string file = "";           // Local file; used with the handler option

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    putenv("_POSIX_OPTION_ORDER=1");    // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'f':
            handler = true;
            handler_name = getopt.optarg;
            break;
        case 'm':
            mime_header = true;
            break;
        case 'w':
            verbose = true;
            break;
        case 'v':
            version = true;
            server_version = getopt.optarg;
            break;
        case 'r':
            cache = true;
            cache_dir = getopt.optarg;
            break;
        case 'u':
            url_given = true;
            url = getopt.optarg;
            break;
        case 'e':
            expr_given = true;
            expr = getopt.optarg;
            break;
        case 'V':{
                cerr << "asciival: " << PACKAGE_VERSION << endl;
                exit(0);
            }
        case 'h':
        case '?':
        default:
            usage();
            exit(1);
            break;
        }
#if 0
    AsciiOutputFactory *aof;
#endif
    try {
        // After processing options, test for errors. There must be a single
        // argument in addition to any given with the options. This will be
        // either a file or a URL, depending on the options supplied and will
        // be the source from which to read the data.
        if (getopt.optind >= argc) {
            usage();
            throw Error("Expected a file or URL argument.");
        }

        if (handler)
            file = argv[getopt.optind];

        // Normally, this filter is called using either a URL _or_ a handler,
        // a file and a URL (the latter is needed for the form which builds
        // the CE and appends it to the URL). However, it's possible to call
        // the filter w/o a handler and pass the URL in explicitly using -u.
        // In that case url_given will be true and the URL will already be
        // assigned to url.
        if (!handler && !url_given)
            url = argv[getopt.optind];

        // Remove the expression from the URL if no expression was given
        // explicitly and the URL is not empty and contains a '?'
        if (!expr_given && !url.empty() && url.find('?') != string::npos) {
            expr = url.substr(url.find('?') + 1);
            url = url.substr(0, url.find('?'));
        }
#if 0
        // See comment below.
        BaseTypeFactory aof;
#else
        AsciiOutputFactory aof;
#endif

        // The version should be read from the handler! jhrg 10/18/05
        DataDDS dds(&aof, "Ascii Data", "DAP2/3.5");

        if (handler) {
            if (verbose)
                cerr << "Reading: " << file << endl;
            string options = "";
            if (version)
                options += string(" -v ") + server_version;
            if (cache)
                options += string(" -r ") + cache_dir;
            read_from_file(dds, handler_name, options, file, expr);
        } else {
            if (verbose)
                cerr << "Reading: " << url << endl;
            read_from_url(dds, url, expr);
        }

        if (mime_header)
            set_mime_text(stdout, dods_data);
#if 0
        // Trick: Since the DataDDS just read has only the stuff matching the
        // CE, set all the send_p flags. The code below must deal with the case
        // where the DDS is not serialized by another process and is instead the
        // whole DDS, so it checks the send_p flag. 
        //
        // NB: This code is here for debugging the handler version of asciival
        // jhrg 11/30/06
        DDS::Vars_iter i = dds.var_begin();
        while (i != dds.var_end()) {
            (*i)->set_send_p(true);
            ++i;
        }

        DataDDS *adds = datadds_to_ascii_datadds(&dds);

        get_data_values_as_ascii(adds, stdout);
        fflush(stdout);
#else
        get_data_values_as_ascii(&dds, stdout);
#endif
    }
    catch(Error & e) {
        DBG(cerr << "Caught an Error object." << endl);
        output_error_object(e);
    }

    catch(exception & e) {
        cerr << "Caught an exception: " << e.what() << endl;
    }

    cout.flush();
    cerr.flush();

    return 0;
}
