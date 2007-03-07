
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

#ifndef _www_output_h
#define _www_output_h

#include <string>
#include <iostream>

#include "BaseType.h"
#include "DDS.h"
#include "DAS.h"

/** Write various parts of the HTML form for a dataset.
    
    @author jhrg. */

class WWWOutput {
private:
    // This pointer is allocated in main() and passed in here so that other
    // functions/methods which use the global instance of WWWOutput name 'wo'
    // can access the DAS object. There are better ways to do this, but this
    // little tool is so simple that I don't think it needs to be
    // rewritten... jhrg

    DAS *d_das;

 protected:
    FILE *d_os;
    int d_attr_rows;
    int d_attr_cols;

    void write_attributes(AttrTable *attr, const string prefix = "");

 public:
    /** Create a WWWOutput. 

	@param os The output stream to which HTML should be sent.
	@param rows The number of rows to show in the attribute textbox
	(default 5).
	@param cols The number of columns to show in the attribute textbox
	(default 70). */
    WWWOutput(FILE *os, int rows = 5, int cols = 70);

#if 0
    /** Set DAS object for this data source. This is used inside the
	print_val() methods. Since those do not have a DAS parameter, the
	specialized methods get it using the matching get_das() accessor. 

	@param das The data soruce's DAS. */
    void set_das(DAS *das) { d_das = das; }

    /** Get the data source's das. 
	@see set_das()
	@return The DAS object. */
    DAS *get_das() const { return d_das; }
#endif

    /** Write out the header for the HTML document. */
    void write_html_header();

    /** Write the disposition section of the HTML page. This section shows
	the URL and provides buttons with which the user can choose the type
	of output.

	@param url The initial URL to display. */
    void write_disposition(string url);

#if 0
    void write_global_attributes(DAS &das);
#endif
    void write_global_attributes(AttrTable &attr);
    
    /** Write the dataset variable list. This is a scrolling select box.

	@deprecated
	@param dds The dataset's DDS. */
    void write_variable_list(DDS &dds);

#if 0
    void write_variable_entries(DAS &das, DDS &dds);
#endif
    void write_variable_entries(DDS &dds);

#if 0
    void write_variable_attributes(BaseType *btp, DAS &das);
#endif
    void write_variable_attributes(BaseType * btp);
};

#endif // __www_output_h
