
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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
 
// (c) COPYRIGHT URI/MIT 1996, 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// The Usage server. Arguments: three arguments; filter options, the dataset
// name and the pathname and `api prefix' of the data server. Returns a HTML
// document that describes what information this dataset contains, special
// characteristics of the server users might want to know and any special
// information that the dataset providers want to make available. jhrg
// 12/9/96

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>

// I've added the pthread code here because this might someday move inside a
// library as a function/object and it should be MT-safe. In the current
// build HAVE_PTHREAD_H is set by configure; not having it makes no practical
// difference. jhrg 6/10/05
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GNURegex.h>

#include <cgi_util.h>
#include <util.h>
#include <DAS.h>

#include <debug.h>
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

namespace dap_usage {
        
// This code could use a real `kill-file' some day - about the same time that
// the rest of the server gets a `rc' file... For the present just see if a
// small collection of regexs match the name.

// The pthread code here is used to ensure that the static objects dim and
// global (in name_in_kill_file() and name_is_global()) are initialized only
// once. If the pthread package is not present when libdap++ is built, this
// code is *not* MT-Safe.

static Regex *dim_ptr;
#if HAVE_PTHREAD_H
static pthread_once_t dim_once_control = PTHREAD_ONCE_INIT;
#endif

static void
init_dim_regex()
{
    // MT-Safe if called via pthread_once or similar
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dim' attributes.
    dim_ptr = &dim;
}

static bool
name_in_kill_file(const string &name)
{
#if HAVE_PTHREAD_H
    pthread_once(&dim_once_control, init_dim_regex);
#else
    if (!dim_ptr)
	init_dim_regex();
#endif

    return dim_ptr->match(name.c_str(), name.length()) != -1;
}

static Regex *global_ptr;
#if HAVE_PTHREAD_H
static pthread_once_t global_once_control = PTHREAD_ONCE_INIT;
#endif

static void
init_global_regex()
{
    // MT-Safe if called via pthread_once or similar
    static Regex global("(.*global.*)|(.*dods.*)", 1);
    global_ptr = &global;
}

static bool
name_is_global(string &name)
{
#if HAVE_PTHREAD_H
    pthread_once(&global_once_control, init_global_regex);
#else
    if (!global_ptr)
	init_global_regex();
#endif

    downcase(name);
    return global_ptr->match(name.c_str(), name.length()) != -1;
}

// write_global_attributes and write_attributes are almost the same except
// that the global attributes use fancier formatting. The formatting could be
// passed in as params, but that would make the code much harder to
// understand. So, I'm keeping this as two separate functions even though
// there's some duplication... 3/27/2002 jhrg
static void
write_global_attributes(ostringstream &oss, AttrTable *attr, 
			const string prefix = "")
{
    if (attr) {
	AttrTable::Attr_iter a;
	for (a = attr->attr_begin(); a != attr->attr_end(); a++) {
	    if (attr->is_container(a))
		write_global_attributes(oss, attr->get_attr_table(a), 
					(prefix == "") ? attr->get_name(a) 
					: prefix + string(".") + attr->get_name(a));
	    else {
		oss << "\n<tr><td align=right valign=top><b>"; 
		if (prefix != "")
		    oss << prefix << "." << attr->get_name(a);
		else
		    oss << attr->get_name(a);
		oss << "</b>:</td>\n";

		int num_attr = attr->get_attr_num(a) - 1;
		oss << "<td align=left>";
		for (int i = 0; i < num_attr; ++i)
		    oss << attr->get_attr(a, i) << ", ";
		oss << attr->get_attr(a, num_attr) << "<br></td></tr>\n";
	    }
	}
    }
}

static void
write_attributes(ostringstream &oss, AttrTable *attr, const string prefix = "")
{
    if (attr) {
	AttrTable::Attr_iter a;
	for (a = attr->attr_begin(); a != attr->attr_end(); a++) {
	    if (attr->is_container(a))
		write_attributes(oss, attr->get_attr_table(a), 
				 (prefix == "") ? attr->get_name(a) 
				 : prefix + string(".") + attr->get_name(a));
	    else {
		if (prefix != "")
		    oss << prefix << "." << attr->get_name(a);
		else
		    oss << attr->get_name(a);
		oss << ": ";

		int num_attr = attr->get_attr_num(a) - 1 ;
		for (int i = 0; i < num_attr; ++i)
		    oss << attr->get_attr(a, i) << ", ";
		oss << attr->get_attr(a, num_attr) << "<br>\n";
	    }
	}
    }
}

/** Given the DAS and DDS, build the HTML* document which contains all the
    global attributes for this dataset. A global attribute is defined here as
    any attribute not bound to variable in the dataset. Thus the attributes
    of `NC_GLOBAL', `global', etc. will be called global attributes if there
    are no variables `NC_GLOBAL', ... in the dataset. If there are variable
    with those names the attributes will NOT be considered `global
    attributes'.

    @brief Build the global attribute HTML* document.
    @return A string object containing the global attributes in human
    readable form (as an HTML* document).
*/
static string
build_global_attributes(DAS &das, DDS &)
{
    bool found = false;
    ostringstream ga;

    ga << "<h3>Dataset Information</h3>\n<center>\n<table>\n";

    for (AttrTable::Attr_iter p = das.var_begin(); p != das.var_end(); p++) {
	string name = das.get_name(p);

	// I used `name_in_dds' originally, but changed to `name_is_global'
	// because aliases between groups of attributes can result in
	// attribute group names which are not in the DDS and are *not*
	// global attributes. jhrg. 5/22/97
	if (!name_in_kill_file(name) && name_is_global(name)) {
	    AttrTable *attr = das.get_table(p);
	    found = true;
	    write_global_attributes(ga, attr, "");
	}
    }

    ga << "</table>\n</center><p>\n";

    if (found)
	return ga.str();

    return "";
}

static string
fancy_typename(BaseType *v)
{
    string fancy;
    switch (v->type()) {
      case dods_byte_c:
	return "Byte";
      case dods_int16_c:
	return "16 bit Integer";
      case dods_uint16_c:
	return "16 bit Unsigned integer";
      case dods_int32_c:
	return "32 bit Integer";
      case dods_uint32_c:
	return "32 bit Unsigned integer";
      case dods_float32_c:
	return "32 bit Real";
      case dods_float64_c:
	return "64 bit Real";
      case dods_str_c:
	return "String";
      case dods_url_c:
	return "URL";
      case dods_array_c: {
	  ostringstream type;
	  Array *a = (Array *)v;
	  type << "Array of " << fancy_typename(a->var()) <<"s ";
	  for (Array::Dim_iter p = a->dim_begin(); p != a->dim_end(); p++) {
	      type << "[" << a->dimension_name(p) << " = 0.." 
		   << a->dimension_size(p, false)-1 << "]";
	  }
	  return type.str();
      }

      case dods_structure_c:
	return "Structure";
      case dods_sequence_c:
	return "Sequence";
      case dods_grid_c:
	return "Grid";
      default:
	return "Unknown";
    }
}

static void
write_variable(BaseType *btp, DAS &das, ostringstream &vs)
{
    vs << "<td align=right valign=top><b>" << btp->name() 
       << "</b>:</td>\n"
       << "<td align=left valign=top>" << fancy_typename(btp)
       << "<br>";

    AttrTable *attr = das.get_table(btp->name());
	    
    write_attributes(vs, attr, "");

    switch (btp->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
	vs << "</td>\n";
	break;

      case dods_structure_c: {
	vs << "<table>\n";
	Structure *sp = dynamic_cast<Structure *>(btp);
	for (Constructor::Vars_iter p = sp->var_begin(); p != sp->var_end(); p++)
	{
	    vs << "<tr>";
	    write_variable((*p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_sequence_c: {
	vs << "<table>\n";
	Sequence *sp = dynamic_cast<Sequence *>(btp);
	for (Constructor::Vars_iter p = sp->var_begin(); p != sp->var_end(); p++)
	{
	    vs << "<tr>";
	    write_variable((*p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_grid_c: {
	  vs << "<table>\n";
	  Grid *gp = dynamic_cast<Grid *>(btp);
	  write_variable(gp->array_var(), das, vs);
	  Grid::Map_iter p;
	  for (p = gp->map_begin(); p != gp->map_end(); p++) {
	      vs << "<tr>";
	      write_variable((*p), das, vs);
	      vs << "</tr>";
	  }
	  vs << "</table>\n";
	  break;
      }

      default:
	throw InternalErr(__FILE__, __LINE__, "Unknown type");
    }
}

/** Given the DAS and the DDS build an HTML table which describes each one of
    the variables by listing its name, datatype and all of its attriutes.

    @brief Build the variable summaries.
    @return A string object containing the variable summary information in
    human readable form (as an HTML* document).
*/

static string
build_variable_summaries(DAS &das, DDS &dds)
{
    ostringstream vs;
    vs << "<h3>Variables in this Dataset</h3>\n<center>\n<table>\n";
    //    vs << "<tr><th>Variable</th><th>Information</th></tr>\n";

    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); p++) {
	vs << "<tr>";
	write_variable((*p), das, vs);
	vs << "</tr>";
    }

    vs << "</table>\n</center><p>\n";

    return vs.str();
}

void
html_header()
{
    fprintf( stdout, "HTTP/1.0 200 OK\n" ) ;
    fprintf( stdout, "XDODS-Server: %s\n", PACKAGE_VERSION ) ;
    fprintf( stdout, "XDAP-Version: %s\n", DAP_VERSION ) ;
    fprintf( stdout, "Content-type: text/html\n" ) ;
    fprintf( stdout, "Content-Description: dods_description\n" ) ;
    fprintf( stdout, "\n" ) ;	// MIME header ends with a blank line
}

/** Build an HTML page that summarizes the information held int eh DDS/DAS.
    This also uses the dataset and server name to lookup extra information
    that the data provider has made available (using libdap's 
    cgi_util.cc:get_user_supplied_docs().
    
    @note This function is faithful to the original server3 'info' response
    in all ways \e except that it does not handle the 'override' document
    feature of that server. This feature was never used outside of testing,
    to the best of our knowledge.
    
    @todo Update this to use the DDX.
    
    @param os Write the HTML to this stream
    @param dds The DDS
    @param das THe DAS
    @param dataset_name Use this name to find dataset-specific info added by
    the provider.
    @param server_name Use this name to find server-specific info. */
void
write_usage_response(FILE *os, DDS &dds, DAS &das, const string &dataset_name,
                     const string &server_name) throw(Error)
{
        // This will require some hacking in libdap; maybe that code should
        // move here? jhrg
        string user_html = get_user_supplied_docs(dataset_name, server_name);

        string global_attrs = build_global_attributes(das, dds);

        string variable_sum = build_variable_summaries(das, dds);

        // Write out the HTML document.

        html_header();

        if (global_attrs.length()) {
            fprintf(os, "%s\n%s\n%s\n%s\n",
                    "<html><head><title>Dataset Information</title></head>",
                    "<body>",
                    global_attrs.c_str(),
                    "<hr>" ) ;
        }

        fprintf( os, "%s\n", variable_sum.c_str() ) ;

        fprintf( os, "<hr>\n" ) ;

        fprintf( os, "%s\n", user_html.c_str() ) ;

        fprintf( os, "</body>\n</html>\n" ) ;
}

} // namespace dap_usage
