
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

// NB: I tried to add a private default ctor to prevent anypne from calling
// it. However, g++ complains about such a ctor because it does not
// initialize the _os reference member. 4/13/99 jhrg

class WWWOutput {
 protected:
    ostream &_os;
    int _attr_rows;
    int _attr_cols;

    void write_attributes(AttrTable *attr, const string prefix = "");

 public:
    /** Create a WWWOutput. 

	@param os The output stream to which HTML should be sent.
	@param rows The number of rows to show in the attribute textbox
	(default 5).
	@param cols The number of columns to show in the attribute textbox
	(default 70). */
    WWWOutput(ostream &os, int rows = 5, int cols = 70);

    /** Write out the header for the HTML document. */
    void write_html_header(bool nph_header);

    /** Write the disposition section of the HTML page. This section shows
	the URL and provides buttons with which the user can choose the type
	of output.

	@param url The initial URL to display. */
    void write_disposition(string url);

    /** Given the DAS, write the HTML which contains all the global
	attributes for this dataset. A global attribute is defined as
	any attribute for which name_in_kill_file() is false and
	name_is_global() is true.

	NB: This mfunc used to scan the DDS and look for attributes which
	matched no variable's name. Such an attribute was considered global.
	However, this proved to be unreliable because some servers create
	attributes which match no variable names exactly and because
	attribute aliases can introduce new attrbute containers which also
	match no variables' names.

	@param das The DAS for the dataset. */
    void write_global_attributes(DAS &das);

    /** Write the dataset variable list. This is a scrolling select box.

	@deprecated
	@param dds The dataset's DDS. */
    void write_variable_list(DDS &dds);

    /** Given the DAS and the DDS, write an HTML table which describes each
	one of the variables by listing its name, datatype and all of its
	attributes.

	@memo Write the variable entries.
	@param das The dataset's DAS.
	@param dds The dataset's DDS. */
    void write_variable_entries(DAS &das, DDS &dds);

    /** Write a variable's attribtute information. 

	@param das The dataset's DAS.
	@param btp A pointer to the variable. */
    void write_variable_attributes(BaseType *btp, DAS &das);
};

/** Write out the HTML for simple type variables. This is used not only for
    single instances of simple types, but also for those types when they
    appear within Structure and Sequence types.

    @param name The name of the varaible. */
void write_simple_variable(ostream &os, const string &name, 
			   const string &type);

/** Return a string which describes the datatype of a DODS variable.

    @param v The variable.
    @return A string describing the variable's type. */
string fancy_typename(BaseType *v);

/** Generate variable names to be used by the JavaScript code. These names
    must not interfere with JavaScript itself, which seems to have an always
    expanding set of reserved words. Maybe that's stopped now (1/25/2001
    jhrg) but the set is large and includes words such as `Location' which is
    also a dataset variable name. By making the JS variable names
    `dods_<var>' the HTML/JS page/code is still readable without stepping on
    JS' reserved words.

    Note that this is a function and not a class member; I included it in the
    WWWOutput class since it seems to fit here. 

    @param dods_name A string that contains the name of a variable.
    @return A name suitable for use in JavaScript code similar to the
    #dods_name# parameter. */

string name_for_js_code(const string &dods_name);

// $Log: WWWOutput.h,v $
// Revision 1.4  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.2.2.2  2002/03/27 14:55:51  jimg
// Added a prefix argument to write_attributes. This is used in recursive calls
// when printing nested attributes.
//
// Revision 1.3  2001/01/26 19:17:36  jimg
// Merged with release-3-2.
//
// Revision 1.2.2.1  2001/01/26 04:04:33  jimg
// Fixed a bug in the JavaScript code. Now the name of the JS variables
// are prefixed by `dods_'. This means that DODS variables whose names are
// also reserved words in JS work break the JS code.
//
// Revision 1.2  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//

#endif // __www_output_h
