
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

static char rcsid[] not_used = {"$Id: www_int.cc,v 1.15 2004/07/08 22:32:19 jimg Exp $"};

#include <stdio.h>

#include <fstream>
#include <string>

#include <GetOpt.h>
#include <Pix.h>
#include <string>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "BaseType.h"
#include "Connect.h"
#include "cgi_util.h"
#include "debug.h"

#include "WWWOutput.h"
#include "javascript.h"		// Try to hide this stuff...

const char *version = PACKAGE_VERSION;

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

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out.

    @author jhrg */
#if 0
static void
output_error_object(Error e)
{
    if (e.OK())
	cout << "Error: " << e.error_message() << endl;
}
#endif
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
    GetOpt getopt (argc, argv, "vVnmH:a:h?");
    int option_char;
#if 0
    bool trace = false;
#endif
    bool verbose = false;
    bool regular_header = false;
    bool nph_header = false;
    string help_location = "http://unidata.ucar.edu/packages/dods/help_files/";
    string admin_name = "";
#if 0
    char *tcode = NULL;
    int topts = 0;
#endif

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

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
	url = new Connect(argv[getopt.optind]);

	url->set_cache_enabled(false); // Server components should not cache...

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
	     << "<form action=\"\">\n"
	     << "<table>\n";
	wo.write_disposition(argv[getopt.optind]);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_global_attributes(global_das);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_variable_entries(global_das, dds);
	cout << "</table></form>\n\n"
	     << "<hr>\n"
	     << "<font size=-1>Tested on Netscape 4.61, 7.1, Internet Explorer 5.0, Mozilla 1.2.1, 1.4, 1.6, Galeon 1.2.7 and Konqueror 3.1-13. Users have reported problems getting the DODS object using IE 6.0 and NS 7.02. However, the other features work fine on those browsers. </font>\n"
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
// Revision 1.15  2004/07/08 22:32:19  jimg
// Merged with relese-3-4-3FCS
//
// Revision 1.13.2.6  2004/07/05 07:47:34  rmorris
// For the last output 'mode' fix, a couple of includes are required.  Added.
//
// Revision 1.13.2.5  2004/07/05 07:27:55  rmorris
// Use binary mode to output under win32.
//
// Revision 1.13.2.4  2004/06/25 21:20:36  jimg
// Added to compatibility note: See bug 652.
//
// Revision 1.14  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.13.2.3  2003/07/28 21:08:24  jimg
// Tested on all the browsers I have; added to list shown in page and moved that
// list to the bottom of the page.
//
// Revision 1.13.2.2  2003/07/11 05:27:34  jimg
// Fixed the build of the javascript.h header from www.js.
//
// Revision 1.13.2.1  2003/05/07 17:13:55  jimg
// Changed to Connect's new ctor and removed old HTTP trace code (that doesn't
// work with libcurl).
//
// Revision 1.13  2003/03/06 18:50:29  jimg
// Replaced call to Connect::disable_cache() with a call to
// Connect::set_cache_enable(false). The disable_cache() method no longer exists.
//
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
