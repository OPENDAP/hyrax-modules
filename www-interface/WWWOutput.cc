
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

static char rcsid[] not_used =
    { "$Id$" };

#include <string>
#include <iostream>
#include <sstream>

#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#include <io.h>
#endif

#include <BaseType.h>
#include <DDS.h>

#include <debug.h>
#include <cgi_util.h>
#include <util.h>

#include "WWWOutput.h"

using namespace std;

#ifdef WIN32
#define getpid _getpid
#define access _access
#define X_OK 00                 //  Simple existance
#endif

WWWOutput::WWWOutput(FILE * os, int rows, int cols) : d_das(0), d_os(os),
    d_attr_rows(rows), d_attr_cols(cols)
{
}

void
WWWOutput::write_html_header()
{
    set_mime_html(d_os, unknown_type, dap_version(), x_plain);
}

void WWWOutput::write_disposition(string url)
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

    fprintf(d_os,
            "<input type=\"button\" value=\"Binary Data Object \" onclick=\"binary_button('dods')\">\n\
<input type=\"button\" value=\"Show Help\" onclick=\"help_button()\">\n\
\n\
<tr>\n\
<td align=\"right\"><h3><a href=\"opendap_form_help.html#data_url\" target=\"help\">Data URL:</a>\n\
</h3>\n\
<td><input name=\"url\" type=\"text\" size=\"%d\" value=\"%s\">\n",
            d_attr_cols, url.c_str());
}

void WWWOutput::write_attributes(AttrTable * attr, const string prefix)
{
    if (attr) {
        for (AttrTable::Attr_iter a = attr->attr_begin();
             a != attr->attr_end(); ++a) {
            if (attr->is_container(a))
                write_attributes(attr->get_attr_table(a),
                                 (prefix == "") ? attr->get_name(a)
                                 : prefix + string(".") + attr->get_name(a));
            else {
                if (prefix != "")
                    fprintf(d_os, "%s.%s: ", prefix.c_str(),
                            attr->get_name(a).c_str());
                else
                    fprintf(d_os, "%s: ", attr->get_name(a).c_str());

                int num_attr = attr->get_attr_num(a) - 1;
                for (int i = 0; i < num_attr; ++i)
                    fprintf(d_os, "%s, ", attr->get_attr(a, i).c_str());
                fprintf(d_os, "%s\n", attr->get_attr(a, num_attr).c_str());
            }
        }
    }
}

/** Given the global attribute table, write the HTML which contains all the 
    global attributes for this dataset. A global attribute is defined in the
    source file DDS.cc by the DDS::transfer_attributes() method.

    @param das The AttrTable with the global attributes. */
void WWWOutput::write_global_attributes(AttrTable &attr)
{
    fprintf(d_os, "<tr>\n\
<td align=\"right\" valign=\"top\"><h3>\n\
<a href=\"opendap_form_help.html#global_attr\" target=\"help\">Global Attributes:</a></h3>\n\
<td><textarea name=\"global_attr\" rows=\"%d\" cols=\"%d\">\n", d_attr_rows, d_attr_cols);

    write_attributes(&attr);

    fprintf(d_os, "</textarea><p>\n\n");
}

void WWWOutput::write_variable_entries(DDS &dds)
{
    // This writes the text `Variables:' and then sets up the table so that
    // the first variable's section is written into column two.
    fprintf(d_os, "<tr>\n\
<td align=\"right\" valign=\"top\">\n\
<h3><a href=\"opendap_form_help.html#dataset_variables\" target=\"help\">Variables:</a></h3>\n\
<td>");

    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); ++p) {
        (*p)->print_val(d_os);

        write_variable_attributes(*p);
        
        fprintf(d_os, "\n<p><p>\n\n");  // End the current var's section
        fprintf(d_os, "<tr><td><td>\n\n");      // Start the next var in column two
    }
}

/** Write a variable's attribtute information. 

    @param btp A pointer to the variable.*/
void WWWOutput::write_variable_attributes(BaseType * btp)
{
    AttrTable &attr = btp->get_attr_table();
    
    // Don't write anything if there are no attributes.
    if (attr.get_size() == 0) {
        DBG(cerr << "No Attributes for " << btp->name() << endl);
        return;
    }

    fprintf(d_os, "<textarea name=\"%s_attr\" rows=\"%d\" cols=\"%d\">\n",
            btp->name().c_str(), d_attr_rows, d_attr_cols);
    write_attributes(&attr);
    fprintf(d_os, "</textarea>\n\n");
}
