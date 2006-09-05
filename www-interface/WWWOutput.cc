
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
// Foundation, 59 Temple Place - Suite 330, Bsston, MA 02111-1307, USA.
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <string>
#include <iostream>
#include <sstream>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#include <process.h>
#include <io.h>
#endif

#include <GNURegex.h>

#include <BaseType.h>
#include <Array.h>
#include <DAS.h>
#include <DDS.h>
#include <InternalErr.h>
//#include <escaping.h>
#include <cgi_util.h>
#include <util.h>

#include "WWWOutput.h"

using namespace std;

static bool name_is_global(string &name);
static bool name_in_kill_file(const string &name);

#ifdef WIN32
#define getpid _getpid
#define access _access
#define X_OK 00  //  Simple existance

// This was part of a system to provide a way to 'drop in' new DAP --> format
// handlers. But only www_int was ever complete enough to be released. Since
// it's always present, it seems silly to dump a huge effort into this code.
// See below in write_dispssition(). Also see Trac ticket #134. jhrg 7/22/05. 
#if 0
char *www_int = "./www_int.exe";
char* dods2ncdf = "./dods2ncdf.exe";
char* dods2hdf4 = "./dods2hdf4.exe";
char* dods2hdf5 = "./dods2hdf5.exe";
char* dods2mat = "./dods2mat.exe";
char* dods2idl = "./dods2idl.exe";
#else
char* www_int = "/usr/local/bin/dap_www_int";
char* dods2ncdf = "dods2ncdf";
char* dods2hdf4 = "dods2hdf4";
char* dods2hdf5 = "dods2hdf5";
char* dods2mat = "dods2mat";
char* dods2idl = "dods2idl";
#endif

#endif

// This code could use a real `kill-file' some day - about the same time that
// the rest of the server gets a `rc' file... For the present just see if a
// small collection of regexs match the name.

static bool
name_in_kill_file(const string &name)
{
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dimension' attributes.

    return dim.match(name.c_str(), name.length()) != -1;
}

static bool
name_is_global(string &name)
{
    static Regex global("(.*global.*)|(.*dods.*)", 1);
    downcase(name);
    return global.match(name.c_str(), name.length()) != -1;
}

WWWOutput::WWWOutput(FILE *os, int rows, int cols):
    d_os(os), _attr_rows(rows), _attr_cols(cols)
{
}

void
WWWOutput::write_html_header()
{
    set_mime_html(d_os, unknown_type, dap_version(), x_plain);
}

void
WWWOutput::write_disposition(string url)
{
    // To get the size to be a function of the image window size, you need to
    // use some JavaScript code to generate the HTML. C++ --> JS --> HTML.
    // 4/8/99 jhrg

    fprintf(d_os, "<tr>\n\
<td align=\"right\">\n\
<h3>\n\
<a href=\"opendap_form_help.html#disposition\" target=\"help\">Action:</a></h3>\n\
<td>\n\
<input type=\"button\" value=\"Get ASCII\" onclick=\"ascii_button()\">\n");

#if 0
    // See the comment above about the 'drop in' format handler support that
    // was never needed. jhrg 7/22/05

    if (acceos(dods2ncdf, X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get netCDF\" onclick=\"binary_button('netcdf')\">\n";

    if (acceos(dods2hdf4, X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get HDF 4\" onclick=\"binary_button('hdf4')\">\n";

    if (acceos(dods2hdf5, X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get HDF 5\" onclick=\"binary_button('hdf5')\">\n";

    if (acceos(dods2mat, X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get MatLAB\" onclick=\"binary_button('mat')\">\n";

    if (acceos(dods2idl, X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get IDL\" onclick=\"binary_button('idl')\">\n";
#endif

    fprintf(d_os,
"<input type=\"button\" value=\"Binary Data Object \" onclick=\"binary_button('dods')\">\n\
<input type=\"button\" value=\"Show Help\" onclick=\"help_button()\">\n\
\n\
<tr>\n\
<td align=\"right\"><h3><a href=\"opendap_form_help.html#data_url\" target=\"help\">Data URL:</a>\n\
</h3>\n\
<td><input name=\"url\" type=\"text\" size=\"%d\" value=\"%s\">\n",
_attr_cols, url.c_str());
}

void
WWWOutput::write_attributes(AttrTable *attr, const string prefix)
{
    if (attr) {
        for (AttrTable::Attr_iter a = attr->attr_begin(); a != attr->attr_end(); ++a) {
	    if (attr->is_container(a))
		write_attributes(attr->get_attr_table(a), 
				 (prefix == "") ? attr->get_name(a) 
				 : prefix + string(".") + attr->get_name(a));
	    else {
                if (prefix != "")
                    fprintf(d_os, "%s.%s: ", prefix.c_str(),  attr->get_name(a).c_str());
                else
                    fprintf(d_os, "%s: ", attr->get_name(a).c_str());

                int num_attr = attr->get_attr_num(a) - 1 ;
                for (int i = 0; i < num_attr; ++i)
                    fprintf(d_os, "%s, ", attr->get_attr(a, i).c_str());
                fprintf(d_os, "%s\n", attr->get_attr(a, num_attr).c_str());
	    }
	}
    }
}

void
WWWOutput::write_global_attributes(DAS &das)
{
    fprintf(d_os,
"<tr>\n\
<td align=\"right\" valign=\"top\"><h3>\n\
<a href=\"opendap_form_help.html#global_attr\" target=\"help\">Global Attributes:</a></h3>\n\
<td><textarea name=\"global_attr\" rows=\"%d\" cols=\"%d\">\n",
_attr_rows, _attr_cols);

    for (AttrTable::Attr_iter p = das.var_begin(); p != das.var_end(); ++p) {
        string name = das.get_name(p);

        if (!name_in_kill_file(name) && name_is_global(name)) {
            AttrTable *attr = das.get_table(p);
            write_attributes(attr);
        }
    }

    fprintf(d_os, "</textarea><p>\n\n");
}

// deprecated

void 
WWWOutput::write_variable_list(DDS &dds)
{
    fprintf(d_os,
"<a href=\"opendap_form_help.html#dataset_variables\" target=\"help\"><h4>Dataset Variables</a>:</h4>\n\
<select name=\"variables\" multiple size=5 onChange=\"variables_obj.var_selection()\">\n");

    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); ++p) {
        fprintf(d_os, "<option value=\"%s\"> %s", (*p)->name().c_str(), 
                (*p)->name().c_str());
    }

    fprintf(d_os, "</select>\n");
}

void
WWWOutput::write_variable_entries(DAS &das, DDS &dds)
{
    // This writes the text `Variables:' and then sets up the table so that
    // the first variable's section is written into column two.
    fprintf(d_os,
"<tr>\n\
<td align=\"right\" valign=\"top\">\n\
<h3><a href=\"opendap_form_help.html#dataset_variables\" target=\"help\">Variables:</a></h3>\n\
<td>");
    
    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); ++p) {
	(*p)->print_val(d_os);
	write_variable_attributes((*p), das);
	fprintf(d_os, "\n<p><p>\n\n");		// End the current var's section
	fprintf(d_os, "<tr><td><td>\n\n");	// Start the next var in column two
    }
}

void
WWWOutput::write_variable_attributes(BaseType *btp, DAS &das)
{
    AttrTable *attr = das.get_table(btp->name());
    // Don't write anything if there are no attributes.
    if (!attr)
	return;

    fprintf(d_os, "<textarea name=\"%s_attr\" rows=\"%d\" cols=\"%d\">\n",
        btp->name().c_str(), _attr_rows, _attr_cols);
    write_attributes(attr);
    fprintf(d_os, "</textarea>\n\n");
}

