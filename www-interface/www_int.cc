
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of www_int, software which returns an HTML form which
// can be used to build a URL to access data from a DAP data server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// asciival is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
// 
// asciival is distributed in the hope that it will be useful, but WITHOUT
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
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

/** 
    @author: jhrg */

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: www_int.cc,v 1.12 2003/01/27 23:53:55 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include <fstream>
#include <strstream>		// Should be sstream! 4/8/99 jhrg
#include <string>

#include <GetOpt.h>
#include <Pix.h>
#include <string>

#include "BaseType.h"
#include "Connect.h"
#include "cgi_util.h"
#include "debug.h"

#include "WWWOutput.h"
#include "javascript.h"		// Try to hide this stuff...

const char *version = WWW_INT_VER;

DAS global_das;
WWWOutput wo(cout);

static void
usage(string name)
{
    cerr << "Usage: " << name << " [options] -- <URL>" << endl
	 << "m: Print a MIME header. Use this with nph- style CGIs.\n"
	 << "n: Print HTTP protocol and status for reply.\n"
	 << "H: Location (URL) of the help file. Should end in '\'.\n"
	 << "a: Server administrator email.\n"
	 << "v: Verbose output. Currently a null option.\n"
	 << "V: Print version information and exit.\n"
	 << "t: HTTP trace information.\n"
	 << "h|?: This meassage.\n";
}

static void
process_trace_options(char *tcode) 
{
#if 0
    char c;
    while ((c = *tcode++))
	switch (c) {
	  case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; break;
	  case 'A': WWWTRACE |= SHOW_APP_TRACE; break;
	  case 'b': WWWTRACE |= SHOW_BIND_TRACE; break;
	  case 'c': WWWTRACE |= SHOW_CACHE_TRACE; break;
	  case 'h': WWWTRACE |= SHOW_AUTH_TRACE; break;
	  case 'i': WWWTRACE |= SHOW_PICS_TRACE; break;
	  case 'k': WWWTRACE |= SHOW_CORE_TRACE; break;
	  case 'l': WWWTRACE |= SHOW_SGML_TRACE; break;
	  case 'm': WWWTRACE |= SHOW_MEM_TRACE; break;
	  case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; break;
	  case 's': WWWTRACE |= SHOW_STREAM_TRACE; break;
	  case 't': WWWTRACE |= SHOW_THREAD_TRACE; break;
	  case 'u': WWWTRACE |= SHOW_URI_TRACE; break;
	  case 'U': WWWTRACE |= SHOW_UTIL_TRACE; break;
	  case 'x': WWWTRACE |= SHOW_MUX_TRACE; break;
	  case 'z': WWWTRACE = SHOW_ALL_TRACE; break;
	  default:
	    cerr << "Unrecognized trace option: `" << *tcode << "'" 
		 << endl;
	    break;
	}
#endif
}

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out.

    @author jhrg */

static void
output_error_object(Error e)
{
    if (e.OK())
	cout << "Error: " << e.error_message() << endl;
}

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

int
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "vVt:nmH:a:h?");
    int option_char;
    bool trace = false;
    bool verbose = false;
    bool regular_header = false;
    bool nph_header = false;
    string help_location = "http://unidata.ucar.edu/packages/dods/help_files/";
    string admin_name = "";
    char *tcode = NULL;
    int topts = 0;

    putenv("_POSIX_OPTION_ORDER=1"); // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF) {
	switch (option_char) {
	  case 'm': regular_header = true; break;
	  case 'H': help_location = getopt.optarg; break;
	  case 'a': admin_name = getopt.optarg; break;
	  case 'n': nph_header = true; break;
	  case 'v': verbose = true; break;
	  case 'V': {
	    cerr << "WWW Interface version: " << version << endl
		 << "DAP version: " << dap_version() << endl; 
	    exit(0);
	  }
	  case 't':
	    trace = true;
	    topts = strlen(getopt.optarg);
	    if (topts) {
		tcode = new char[topts + 1];
		strcpy(tcode, getopt.optarg); 
		process_trace_options(tcode);
		delete tcode;
	    }
	    break;
	  case 'h':
	  case '?':
	  default:
	    usage((string)argv[0]); exit(1); break;
	}
    }

    Connect *url = 0;

    // Only process one URL; throw an Error object if more than one is given.
    // 10/8/2001 jhrg
    if (argc > getopt.optind+1) {
	string msg = 
	    "Error: more than one URL was supplied to www_int.\n";
	throw Error(msg);
    }

    try {
	url = new Connect(argv[getopt.optind], trace);

	url->disable_cache();	// Server components should not cache...

	if (url->is_local()) {
	    string msg = "Error: URL `";
	    msg += string(argv[getopt.optind]) + "' is local.\n";
	    throw Error(msg);
	}

	url->request_das();	// These throw on error.
	url->request_dds();

	global_das = url->das();
	DDS dds = url->dds();

	if (regular_header || nph_header)
	    wo.write_html_header(nph_header);

	cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
	     << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n"
	     << "<html><head><title>DODS Dataset Query Form</title>\n"
	     << "<base href=\"" << help_location << "\">\n"
	     << "<script type=\"text/javascript\">\n"
	     << "<!--\n"
	    // Javascript code here
	     << java_code << "\n"
	     << "DODS_URL = new dods_url(\"" << argv[getopt.optind] << "\");\n"
	     << "// -->\n"
	     << "</script>\n"
	     << "</head>\n" 
	     << "<body>\n"
	     << "<p><h2 align='center'>DODS Dataset Access Form</h2>\n"
	     << "<hr>\n"
	     << "<font size=-1>Tested on Netscape 4.61 and Internet Explorer 5.00.</font>\n"
	     << "<hr>\n"
	     << "<form action=\"\">\n"
	     << "<table>\n";
	wo.write_disposition(argv[getopt.optind]);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_global_attributes(global_das);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_variable_entries(global_das, dds);
	cout << "</table></form>\n\n"
	     << "<hr>\n\n";
	if (admin_name != "") {
	    cout << "<address>Send questions or comments to: <a href=\"mailto:"
		 << admin_name << "\">" << admin_name << "</a></address>\n\n"
		 << "<address>For general help with DODS, see: "
		 << "<a href=\"http://unidata.ucar.edu/packages/dods/\">"
		 << "http://unidata.ucar.edu/packages/dods/</a></address>\n\n";
	}
	else {
	    cout << "<address>Send questions or comments to: <a href=\"mailto:support@unidata.ucar.edu\">support@unidata.ucar.edu</a></address>"
		 << "</body></html>\n";
	}
    }
    catch (Error &e) {
	string error_msg = e.get_error_message();
	cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
	     << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n"
	     << "<html><head><title>DODS Error</title>\n"
	     << "<base href=\"" << help_location << "\">\n"
	     << "</head>\n" 
	     << "<body>\n"
	     << "<h3>Error building the DODS dataset query form</h3>:\n"
	     << error_msg
	     << "<hr>\n";

	return 1;
	     
    }

    return 0;
}

// $Log: www_int.cc,v $
// Revision 1.12  2003/01/27 23:53:55  jimg
// Merged with release-3-2-7.
//
// Revision 1.8.2.8  2002/09/05 22:27:29  pwest
// Removed includes to SLList and DLList. These are not necessary and no longer
// supported.
//
// Revision 1.8.2.7  2002/06/21 00:31:42  jimg
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
// Revision 1.8.2.6  2002/02/04 17:30:41  jimg
// I removed a lot of code that was duplicated from src/dap/usage.cc. I moved that code into cgi_util.cc/h.
//
// Revision 1.8.2.5  2002/01/17 06:35:55  jimg
// Added a call to Connect::disable_cache() so that this tool won't cache
// responses.
//
// Revision 1.11  2001/10/14 01:56:09  jimg
// Merged with release-3-2-5.
//
// Revision 1.8.2.4  2001/10/09 00:57:24  jimg
// Now returns 1 on error. Uses catch statements to grab error returns from
// methods of Connect. This fixes bug 300 (at least for www_int). A future
// version of the dispatch mechanism might actually use the error messages from
// this program...
//
// Revision 1.10  2001/09/28 23:51:32  jimg
// Merged with 3.2.4.
//
// Revision 1.8.2.3  2001/09/27 05:41:31  jimg
// Added debug.h.
//
// Revision 1.9  2001/01/26 19:17:36  jimg
// Merged with release-3-2.
//
// Revision 1.8.2.2  2001/01/26 04:04:33  jimg
// Fixed a bug in the JavaScript code. Now the name of the JS variables
// are prefixed by `dods_'. This means that DODS variables whose names are
// also reserved words in JS work break the JS code.
//
// Revision 1.8.2.1  2000/11/10 23:43:09  jimg
// Build changes.
//
// Revision 1.8  2000/11/09 21:04:37  jimg
// Merged changes from release-3-1. There was a goof and a bunch of the
// changes never made it to the branch. I merged the entire branch.
// There maybe problems still...
//
// Revision 1.7  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.6  2000/10/03 18:41:44  jimg
// Added exception handler to main().
//
// Revision 1.5  2000/10/02 22:42:06  jimg
// Replaced DVR constant from config_dap.h with the dap_version function
//
// Revision 1.4  1999/11/05 00:12:23  jimg
// Result of merge with 3-1-2
//
// Revision 1.3.2.1  1999/08/17 22:22:06  jimg
// Final changes for the 3.1 build
//
// Revision 1.3  1999/06/22 17:14:19  jimg
// Added links for help location.
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:06  jimg
// First version
