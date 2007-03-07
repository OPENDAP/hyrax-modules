
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of www_int, software which returns an HTML form which
// can be used to build a URL to access data from a DAP data server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// www_int is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
// 
// www_int is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
// 
// You should have received a copy of the GNU General Public License along
// with GCC; see the file COPYING. If not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

/** 
    @author: jhrg */

#include "config.h"

static char rcsid[] not_used =
    { "$Id$" };

#include <stdio.h>

#include <sstream>
#include <string>

#include <GetOpt.h>
#include <string>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "BaseType.h"
#include "Connect.h"
#include "BaseTypeFactory.h"
#include "cgi_util.h"
#include "debug.h"

#include "WWWOutput.h"
#include "WWWOutputFactory.h"

#include "get_html_form.h"

using namespace dap_html_form;

const char *version = PACKAGE_VERSION;

static void usage(string name)
{
    cerr << "Usage: " << name << " [options] -- <url> | <file>\n"
        << "m: Print a MIME header. Use this with nph- style CGIs.\n"
        << "n: Print HTTP protocol and status for reply.\n"
        << "f <handler pathname>: Use a local handler instead of reading from a URL"
        << endl 
        << "H: Location (URL) of the help file. Should end in '\'.\n"
        << "a <email>: Server administrator email.\n"
        << "v <string>: Read the server version information.\n"
        << "r <dir>: Path to the cache directory.\n"
        << "u <url>: URL to the referenced data source.\n"
        << "V: Print version information and exit.\n" 
        << "h|?: This meassage.\n";
}

static void read_from_url(DAS & das, DDS & dds, const string & url)
{
    Connect *c = new Connect(url);

    c->set_cache_enabled(false);        // Server components should not cache...

    if (c->is_local()) {
        string msg = "Error: URL `";
        msg += string(url) + "' is local.\n";
        throw Error(msg);
    }

    c->request_das(das);
    c->request_dds(dds);

    delete c;
    c = 0;
}

static void
read_from_file(DAS & das, DDS & dds, const string & handler,
               const string & options, const string & file)
{
    string command = handler + " -o DAS " + options + " \"" + file + "\"";

    DBG(cerr << "DAS Command: " << command << endl);

    FILE *in = popen(command.c_str(), "r");

    if (in && remove_mime_header(in)) {
        das.parse(in);
        pclose(in);
    }

    command = handler + " -o DDS " + options + " \"" + file + "\"";
    DBG(cerr << "DDS Command: " << command << endl);

    in = popen(command.c_str(), "r");

    if (in && remove_mime_header(in)) {
        dds.parse(in);
        pclose(in);
    }
}

/* How this code is called from server3
 * 
 *     } elsif ( $self->ext() eq "html" ) {
        $options    = "-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir();
        }

        @command = ( $self->www_int(), $options, "-m", "-n", 
                     "-u", $self->url_text(),
                     "-f", $self->handler(), 
                     "--", $self->filename() ); #. "?" . $self->query() );
 */

/** 
    Build an HTML page which both describes the data and provides a way for
    users to query the dataset. The page prompts for the input of query
    parameters and builds a URL which matches those parameters. 

    Since this HTML page should display both DDS and DAS information it makes
    little sense to have it read from standard input. Thus the only way to
    get output is with the URL of a dataset passed as a command line
    argument. Note that the software will accept only one argument and ignore
    others (printing a warning). At some point in the future this program
    might accept several URLs and create an page whcih reads from several
    datasets at once.

    @author jhrg */

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "v:r:u:Vnmf:H:a:h?");
    int option_char;
    bool version = false;
    bool cache = false;
    bool regular_header = false;
    bool nph_header = false;
    bool handler = false;
    bool url_given = false;
    // Note that the help file is stored as part of the web page source and
    // not with this source code. If this URL is edited, may sure to also
    // edit the corresponding URL in www.js. jhrg 7/21/05
    string help_location =
        "http://www.opendap.org/online_help_files/opendap_form_help.html";
    string admin_name = "";
    string handler_name = "";
    string server_version = "";
    string cache_dir = "/usr/tmp";
    string url = "";
    string file = "";           // Local file; used with the handler option

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    putenv("_POSIX_OPTION_ORDER=1");    // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF) {
        switch (option_char) {
        case 'm':
            regular_header = true;
            break;
        case 'f':
            handler = true;
            handler_name = getopt.optarg;
            break;
        case 'H':
            help_location = getopt.optarg;
            break;
        case 'a':
            admin_name = getopt.optarg;
            break;
        case 'n':
            nph_header = true;
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
        case 'V':{
                cerr << "WWW Interface version: " << version << endl
                    << "DAP version: " << dap_version() << endl;
                exit(0);
            }
        case 'h':
        case '?':
        default:
            usage((string) argv[0]);
            exit(1);
            break;
        }
    }

    try {
        // After processing options, test for errors. There must be a single
        // argument in addition to any given with the options. This will be
        // either a file or a URL, depending on the options supplied and will
        // be the source from whic to read the data.
        if (getopt.optind >= argc) {
            usage((string) argv[0]);
            throw
                Error
                ("Internal configuration error: Expected a file or URL argument.");
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

        // Only process one URL/file; throw an Error object if more than one is
        // given. 10/8/2001 jhrg
        if (argc > getopt.optind + 1)
            throw
                Error
                ("Internal configuration error: More than one URL was supplied to www_int.");

        if (handler && !url_given)
            throw
                Error
                ("Internal configuration error: Handler supplied but no matching URL given.");

        WWWOutputFactory wof;
        DDS dds(&wof, "WWW Interface");

        DAS das;

        if (handler) {
            string options = "";
            if (version)
                options += string(" -v ") + server_version;
            if (cache)
                options += string(" -r ") + cache_dir;
            read_from_file(das, dds, handler_name, options, file);
        } else
            read_from_url(das, dds, url);

        dds.transfer_attributes(&das);
        
        write_html_form_interface(stdout, &dds, //&das, 
                                  url,
                                  regular_header || nph_header,
                                  admin_name, help_location);
    }
    catch(Error & e) {
        string error_msg = e.get_error_message();
        cout <<
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
            << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n" <<
            "<html><head><title>DODS Error</title>\n" << "<base href=\"" <<
            help_location << "\">\n" << "</head>\n" << "<body>\n" <<
            "<h3>Error building the DODS dataset query form</h3>:\n" <<
            error_msg << "<hr>\n";

        return 1;
    }

    return 0;
}
