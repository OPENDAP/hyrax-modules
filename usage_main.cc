
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2006 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>

#include <DAS.h>

#include <escaping.h>
#include <GNURegex.h>
#include <cgi_util.h>
#include <util.h>
#include <debug.h>

using namespace libdap ;

#include "usage.h"

using namespace std;
using namespace dap_usage;

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#include <io.h>
#include <fcntl.h>
#endif

#ifdef WIN32
#define RETURN_TYPE void
#else
#define RETURN_TYPE int
#endif 

static void
usage(char *argv[])
{
    cerr << argv[0] << " <options> <filename> <CGI prefix>" << endl
         << "Takes three required arguments; command options to be" << endl
         << "passed to the filter programs, the dataset filename and" << endl
         << "the directory and api prefix for the filter program." << endl; 
}

// This program is passes a collection of Unix-style options, the pathname
// of the data source and the pathname of the handler.
RETURN_TYPE
main(int argc, char *argv[])
{
    if (argc != 4) {
        usage(argv);
        exit(1);
    }

    string options = argv[1];

    string name = argv[2];
    string doc;

    if (found_override(name, doc)) {
        html_header( stdout );
        fprintf( stdout, "%s", doc.c_str() ) ;
        exit(0);
    }

    // The site is not overriding the DAS/DDS generated information, so read
    // the DAS, DDS and user supplied documents. 

    string handler = argv[3];

    Regex handler_allowed("[-a-zA-Z_]+");
    if (!handler_allowed.match(handler.c_str(), handler.length()))
        throw Error("Invalid input (1)");
    Regex options_allowed("[-a-zA-Z_]+");
    if (!options_allowed.match(options.c_str(), options.length()))
        throw Error("Invalid input (2)");
        
    // The file paramter (data source name, really) may have escape characters
    // (DODSFilter::initialize calls www2id()) so it's called here and the 
    // resulting string is sanitized. I believe that the only escaped 
    // character allowed is a space...
    Regex name_allowed("[-a-zA-Z0-9_%]+");
    string unesc_name = www2id(name, "%", "%20");
    if (!name_allowed.match(unesc_name.c_str(), unesc_name.length()))
        throw Error("Invalid input (3)");

    DAS das;
    string command = handler + " -o DAS " + options + " \""
        + unesc_name + "\"";

    DBG(cerr << "DAS Command: " << command << endl);

    FILE *in = 0;
    try {
        in = popen(command.c_str(), "r");
        if (in && remove_mime_header(in)) {
            das.parse(in);
            pclose(in);
        }

        BaseTypeFactory factory;
        DDS dds(&factory);
        string command = handler + " -o DDS " + options + " \"" + name + "\"";
        DBG(cerr << "DDS Command: " << command << endl);

        in = popen(command.c_str(), "r");

        if (in && remove_mime_header(in)) {
            dds.parse(in);
            pclose(in);
        }

        // Build the HTML* documents.
        write_usage_response(stdout, dds, das, name, handler);
    }
    catch (Error &e) {
        pclose(in);
        
        string error_msg = e.get_error_message();
        fprintf( stdout, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n" ) ;
        fprintf( stdout, "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n") ;
        fprintf( stdout, "<html><head><title>DODS Error</title>\n" ) ;
        fprintf( stdout, "</head>\n" ) ;
        fprintf( stdout, "<body>\n" ) ;
        fprintf( stdout, "<h3>Error building the DODS dataset usage repsonse</h3>:\n" ) ;
        fprintf( stdout, "%s", error_msg.c_str() ) ;
        fprintf( stdout, "<hr>\n" ) ;
        
        return EXIT_FAILURE;
    }
    catch (exception &e) {
        pclose(in);

        string error_msg = e.what();
        fprintf( stdout, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n" ) ;
        fprintf( stdout, "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n") ;
        fprintf( stdout, "<html><head><title>DODS Error</title>\n" ) ;
        fprintf( stdout, "</head>\n" ) ;
        fprintf( stdout, "<body>\n" ) ;
        fprintf( stdout, "<h3>Error building the DODS dataset usage repsonse</h3>:\n" ) ;
        fprintf( stdout, "%s", error_msg.c_str() ) ;
        fprintf( stdout, "<hr>\n" ) ;
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
