
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
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

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
    data. It should also be smart enough to sort or Error objects and throw
    them, something the code here (and also in Connect::process_data()) does
    not do.

    @author: jhrg */

#include "config_asciival.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>

#include <string>
#include <iostream>

using namespace std;

#include <GetOpt.h>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "BaseType.h"
#include "Connect.h"
#include "name_map.h"
#include "cgi_util.h"
#include "debug.h"

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

name_map names;
bool translate = false;

static void
usage()
{
    cerr << "Usage: \n"
	 << " [mnhwvruVt] -- [<url> | <file> ]\n" 
	 << "       m: Output a MIME header.\n" 
	 << "       n: Turn on name canonicalization.\n"
	 << "       f <handler pathname>: Use a local handler instead of reading from a URL\n"
	 << "          This assumes a local file, not a url.\n"
	 << "       w: Verbose (wordy) output.\n"
	 << "       v <string>: Read the server version information.\n"
	 << "       u <url>: URL to the referenced data source.\n"
	 << "       r <dir>: Path to the cache directory.\n"
	 << "       e <expr>: Constraint expression.\n"
	 << "       V: Print the version number and exit.\n" 
	 << "<url> may be a true URL, which asciival will dereference,\n" 
	 << "it may be a local file or it may be standard input.\n"
	 << "In the later case use `-' for <url>.\n";
}

static void
process_per_url_options(int &i, int argc, char *argv[], bool verbose = false)
{
    names.delete_all();        // Clear the global name map for this URL.
  
    // Test for per-url option. Set variables accordingly.
    while (i < argc && argv[i+1] && argv[i+1][0] == '-')
       switch (argv[++i][1]) {
         case 'r':
           ++i;        // Move past option to argument.
           if (verbose)
               cerr << "  Renaming: " << argv[i] << endl;
           // Note that NAMES is a global variable so that all the
           // writeval() mfuncs can access it without having to pass
           // it into each function call.
           names.add(argv[i]);
           break;

         default:
	   // if this was an option
	   if (argv[i+1][0] == '-') {
	       string msg = "Unknown option `";
	       msg += argv[i][1] + "' paired with URL has been ignored.";
	       throw Error(msg);
	   }
	   else {		// not an option
	       throw Error("More than one URL of file was supplied to dap_asciival.");
	   }
           break;
       }
}

static void
read_from_url(DataDDS &dds, const string &url, const string &expr)
{
    Connect *c = new Connect(url);

    c->set_cache_enabled(false); // Server components should not cache...

    if (c->is_local()) {
	string msg = "Error: URL `";
	msg += string(url) + "' is local.\n";
	throw Error(msg);
    }

    c->request_data(dds, expr);

    delete c; c = 0;
}

static void
read_from_file(DataDDS &dds, const string &handler, 
	       const string &options, const string &file,
	       const string &expr)
{
    string command = handler + " -o dods " + options 
        + " -e " + "\"" + expr  + "\""
	+ " \"" + file + "\"";

    DBG(cerr << "DDS Command: " << command << endl);

    FILE *in = popen(command.c_str(), "r");
    // Cheat: Instead of using our own code, use code from Connect. Really we should
    // make a FILEConnect (just like there's an HTTPConnnect) to process inputs
    // from files. For now, use the read_data() method. jhrg 2/9/06
    Connect c("local_pipe");
    c.read_data(dds, in);
    pclose(in);

#if 0
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
	catch(Error &e) {
            if (xdr_stream)
	       delete_xdrstdio(xdr_stream);
	    throw e;
	}

	delete_xdrstdio(xdr_stream);

	pclose(in);
    }
#endif
}

static void
process_data(DDS *dds)
{
    cout << "Dataset: " << dds->get_dataset_name() << endl;

    DDS::Vars_iter i = dds->var_begin();
    while (i != dds->var_end()) {
	dynamic_cast<AsciiOutput &>(**i++).print_ascii(stdout);
	cout << endl;
    }
}

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out (since that will confuse loaddods).

    @author jhrg */

static void
output_error_object(const Error &e)
{
    if (e.OK())
	cout << "Error: " << e.get_error_message() << endl;
}

// Read a DODS data object. The object maybe specified by a URL (which will
// be dereferenceed using Connect, it maybe read from a file or it maybe read
// from stdin. Use `-' in the command line to indicate the next input should
// be read from standard input.

int
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "nf:mv:r:u:e:wVh?");
    int option_char;
    bool verbose = false;
    bool translate = false;
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
    string file = "";		// Local file; used with the handler option

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    putenv("_POSIX_OPTION_ORDER=1"); // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'n': translate = true; break;
	  case 'f': handler = true; handler_name = getopt.optarg; break;
	  case 'm': mime_header = true; break;
	  case 'w': verbose = true; break;
	  case 'v': version = true; server_version = getopt.optarg; break;
	  case 'r': cache = true; cache_dir = getopt.optarg; break;
	  case 'u': url_given = true; url = getopt.optarg; break;
	  case 'e': expr_given = true; expr = getopt.optarg; break;
	  case 'V': {cerr << "asciival: " << PACKAGE_VERSION << endl; exit(0);}
	  case 'h':
	  case '?':
	  default:
	    usage(); exit(1); break;
	}

    AsciiOutputFactory *aof;
    try {
	// After processing options, test for errors. There must be a single
	// argument in addition to any given with the options. This will be
	// either a file or a URL, depending on the options supplied and will
	// be the source from whic to read the data.
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
	    expr = url.substr(url.find('?')+1);
	    url = url.substr(0, url.find('?'));
	}

	aof = new AsciiOutputFactory;
        // The version should be read from the handler! jhrg 10/18/05
	DataDDS dds(aof, "Ascii Data", "DAP2/3.5");

	if (handler) {
	    if (verbose)
		cerr << "Reading: " << file << endl;
	    string options = "";
	    if (version)
		options += string(" -v ") + server_version;
	    if (cache)
		options += string(" -r ") + cache_dir;
	    read_from_file(dds, handler_name, options, file, expr);
	}
	else {
	    if (verbose)
		cerr << "Reading: " << url << endl;
	    process_per_url_options(getopt.optind, argc, argv, verbose);
	    read_from_url(dds, url, expr);
	}

        if (mime_header)
	    set_mime_text(stdout, dods_data);

	process_data(&dds);
	delete aof; aof = 0;
    }
    catch (Error &e) {
	DBG(cerr << "Caught an Error object." << endl);
	output_error_object(e);
    }
        
    catch (exception &e) {
	cerr << "Caught an exception: " << e.what() << endl;
    }

    cout.flush();
    cerr.flush();

    return 0;
}

// $Log: ascii_val.cc,v $
// Revision 1.20  2004/07/08 22:18:25  jimg
// Merged with release-3-4-3FCS
//
// Revision 1.16.2.4  2004/07/05 07:47:59  rmorris
// For the last output 'mode' fix, a couple of includes are required.  Added.
//
// Revision 1.16.2.3  2004/07/05 07:27:30  rmorris
// Use binary mode to output under win32.
//
// Revision 1.16.2.2  2003/05/08 00:10:09  jimg
// Removed code that commented out.
// $Log: ascii_val.cc,v $
// Revision 1.20  2004/07/08 22:18:25  jimg
// Merged with release-3-4-3FCS
//
// Revision 1.19  2004/01/28 16:27:30  jimg
// Corrected use of Connect. This code was using a constructor that is no longer
// present. The old ctor was present when the software used libwww.
//
// Revision 1.18  2004/01/26 17:38:05  jimg
// Removed List from the DAP
//
// Revision 1.17  2003/05/08 21:08:33  pwest
// added include for iostream and using statements to compile
//
// Revision 1.16  2003/03/06 18:52:10  jimg
// Replaced call to Connect::disable_cache() with a call to
// Connect::set_cache_enable(false). The disable_cache() method no longer exists.
//
// Revision 1.15  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.13.4.6  2002/12/23 23:23:03  jimg
// Removed unused XDR * pointer from the process_data() function. This changes
// was made to accommodate a change in Connect.
//
// Revision 1.13.4.5  2002/07/05 23:58:23  jimg
// I switched to the local config header (config_asciival.h) from the DAP's
// header. I removed a lot of code that was specific to libwww's trace system. I
// changed they way the code uses Connect so that the new methods are used in
// place of the old ones.
//
// Revision 1.13.4.4  2002/06/21 00:31:41  jimg
// I changed many files throughout the source so that the 'make World' build
// works with the new versions of Connect and libdap++ that use libcurl.
// Most of these changes are either to Makefiles, configure scripts or to
// the headers included by various C++ files. In a few places the oddities
// of libwww forced us to hack up code and I've undone those and some of the
// clients had code that supported libwww's generous tracing capabilities
// (that's one part of libwww I'll miss); I had to remove support for that.
// Once this code compiles and more work is done on Connect, I'll return to
// each of these changes and polish them.
//
// Revision 1.13.4.3  2002/01/17 00:45:06  jimg
// I added a call to Connect::disable_cache(). This will suppress use of the
// cache which is important since this `client' is used as part of the DODS
// server and it seems to cause problems whenthe server caches DataDDS
// objects (among other things). I think servers should not cache, in general.
//
// Revision 1.14  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.13.4.2  2001/09/27 03:10:36  jimg
// Minor fix to a DBG call.
//
// Revision 1.13.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.13  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.12  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.11  1999/07/24 00:10:28  jimg
// Merged the release-3-0-2 branch
//
// Revision 1.10  1999/05/25 18:47:11  jimg
// Merged Nathan's fixes for the -t option and some debugging instrumentation.
//
// Revision 1.9  1999/05/18 20:59:32  jimg
// Removed name_from_url(...) since int is never used.
//
// Revision 1.8  1999/04/30 17:06:56  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.7  1999/03/24 06:23:43  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.6  1998/09/16 23:30:26  jimg
// Change process_data() so that it calls print_all_vals() for Structure.
//
// Revision 1.5  1998/08/01 01:31:36  jimg
// Fixed a bug in process_data() where deserialize() was not called before the
// call to print_all_vals().
//
// Revision 1.4  1998/07/30 19:05:54  jimg
// Fixed the call to usage; passing a char * invoked cgi-util.cc:usage which
// was not what we wanted. Also added help about the -m option.
//
// Revision 1.3  1998/03/19 23:26:03  jimg
// Added code to write out error messages read from the server.
//
// Revision 1.2  1998/03/16 19:45:09  jimg
// Added mime header output. See -m.
//
// Revision 1.1  1998/03/16 18:30:19  jimg
// Added
