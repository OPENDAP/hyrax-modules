
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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
 
#ifdef __GNUG__
#pragma implementation
#endif

#include "config_ais_tool.h"

static char rcsid[] not_used = {"$Id$"};

#include <GetOpt.h>

#include "AISDODSFilter.h"
#include "debug.h"

AISDODSFilter::AISDODSFilter(int argc, char *argv[]) 
{
    initialize(argc, argv);
}

/** This method is used to process command line options feed into the filter.
    This is a virtual method and is called by DODSFilter's constructor. This
    version uses the code in the DODSFilter version plus some new code. This
    is probably \i not the best way to handle this sort of thing, but
    splitting up the option processing is hard to do with GetOpt. */
int
AISDODSFilter::process_options(int argc, char *argv[]) throw(Error)
{
    DBG(cerr << "Entering process_options... ");

    d_object = unknown_type;
    d_ais_db = "";

    GetOpt getopt (argc, argv, "asDB:Vce:v:d:f:r:t:l:h?");
    int option_char;
    while ((option_char = getopt()) != EOF) {
	switch (option_char) {
	  case 'a': d_object = dods_das; break;
	  case 's': d_object = dods_dds; break;
	  case 'D': d_object = dods_data; break;
	  case 'B': d_ais_db = getopt.optarg; break;
	    // case 'V': {cerr << "ais_tool: " << VERSION << endl; exit(0);}
	  case 'c': d_comp = true; break;
	  case 'e': d_ce = getopt.optarg; break;
	  case 'v': d_cgi_ver = getopt.optarg; break;
#if 0
	  case 'V': d_ver = true; break;
#endif
	  case 'd': d_anc_dir = getopt.optarg; break;
	  case 'f': d_anc_file = getopt.optarg; break;
	  case 'r': d_cache_dir = getopt.optarg; break;
	  case 'l': 
	    d_conditional_request = true;
	    d_if_modified_since 
		= static_cast<time_t>(strtol(getopt.optarg, NULL, 10));
	    break;
	  default: d_bad_options = true; break;
	}
    }

    DBGN(cerr << "exiting." << endl);

    return getopt.optind;	// Index of next option.
}
