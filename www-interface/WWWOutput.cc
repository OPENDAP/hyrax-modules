
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

// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWOutput.cc,v 1.14 2004/01/28 16:47:49 jimg Exp $"};

#include <string>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include "Regex.h"

#include "BaseType.h"
#include "Array.h"
#include "DAS.h"
#include "DDS.h"
#include "InternalErr.h"
#include "escaping.h"
#include "util.h"

#include "WWWOutput.h"

using namespace std;

static bool name_is_global(string &name);
static bool name_in_kill_file(const string &name);

const string allowable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

// This function adds some text to the variable name so that conflicts with
// JavaScript's reserved words and other conflicts are avoided (or
// minimized...) 7/13/2001 jhrg
string
name_for_js_code(const string &dods_name)
{
    pid_t pid = getpid();
    ostringstream oss;
    // Calling id2www with a different set of allowable chars gets an
    // identifier with chars allowable for JavaScript. Then turn the `%' sign
    // into an underscore.
    oss << "org_dods_dcz" << pid 
	<< esc2underscore(id2www(dods_name, allowable));// << ends;
    return oss.str();
}

WWWOutput::WWWOutput(ostream &os, int rows, int cols):
    _os(os), _attr_rows(rows), _attr_cols(cols)
{
}

void
WWWOutput::write_html_header(bool nph_header)
{
    if (nph_header)
	_os << "HTTP/1.0 200 OK" << endl;
    _os << "XDODS-Server: " << dap_version() << endl;
    _os << "Content-type: text/html" << endl; 
    _os << "Content-Description: dods_form" << endl;
    _os << endl;		// MIME header ends with a blank line
}

void
WWWOutput::write_disposition(string url)
{
    // To get the size to be a function of the image window size, you need to
    // use some JavaScript code to generate the HTML. C++ --> JS --> HTML.
    // 4/8/99 jhrg

    _os << "<tr>\n\
<td align=\"right\"><h3><a href=\"dods_form_help.html#disposition\" valign=\"bottom\">Action:</a></h3>\n";
    _os << "<td>";

    if (access("./asciival", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get ASCII\" onclick=\"ascii_button()\">\n";

    if (access("./dods2ncdf", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get netCDF\" onclick=\"binary_button('netcdf')\">\n";

    if (access("./dods2hdf4", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get HDF 4\" onclick=\"binary_button('hdf4')\">\n";

    if (access("./dods2hdf5", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get HDF 5\" onclick=\"binary_button('hdf5')\">\n";

    if (access("./dods2mat", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get MatLAB\" onclick=\"binary_button('mat')\">\n";

    if (access("./dods2idl", X_OK) == 0)
	_os << "<input type=\"button\" value=\"Get IDL\" onclick=\"binary_button('idl')\">\n";

    _os << "<input type=\"button\" value=\"Get DODS Data Object \" onclick=\"binary_button('dods')\">\n\
<input type=\"button\" value=\"Show Help\" onclick=\"help_button()\">\n\
\n\
<tr>\n\
<td align=\"right\"><h3><a href=\"dods_form_help.html#data_url\" valign=\"bottom\">Data URL:</a>\n\
</h3>\n\
<td><input name=\"url\" type=\"text\" size=" << _attr_cols << " value=\"" 
<< url << "\">"; 
}

#if 0
//I cut the following from the above.
//<input type=\"button\" value=\"Send to Program\" onclick=\"program_button()\">
#endif

void
WWWOutput::write_attributes(AttrTable *attr, const string prefix)
{
    if (attr) {
	for (Pix a = attr->first_attr(); a; attr->next_attr(a)) {
	    if (attr->is_container(a))
		write_attributes(attr->get_attr_table(a), 
				 (prefix == "") ? attr->get_name(a) 
				 : prefix + string(".") + attr->get_name(a));
	    else {
		if (prefix != "")
		    _os << prefix << "." << attr->get_name(a) << ": ";
		else
		    _os << attr->get_name(a) << ": ";

		int num_attr = attr->get_attr_num(a) - 1 ;
		for (int i = 0; i < num_attr; ++i)
		    _os << attr->get_attr(a, i) << ", ";
		_os << attr->get_attr(a, num_attr) << endl;
	    }
	}
    }
}

void
WWWOutput::write_global_attributes(DAS &das)
{
    _os << \
"<tr>\n\
<td align=\"right\" valign=\"top\"><h3>\n\
<a href=\"dods_form_help.html#global_attr\">Global Attributes:</a></h3>\n\
<td><textarea name=\"global_attr\" rows=" << _attr_rows << " cols=" 
<< _attr_cols << ">\n";

    for (Pix p = das.first_var(); p; das.next_var(p)) {
	string name = das.get_name(p);

	if (!name_in_kill_file(name) && name_is_global(name)) {
	    AttrTable *attr = das.get_table(p);
	    write_attributes(attr);
	}
    }

    _os << "</textarea><p>\n\n";
}

// deprecated

void 
WWWOutput::write_variable_list(DDS &dds)
{
    _os << \
"<a href=\"dods_form_help.html#dataset_variables\"><h4>Dataset Variables</a>:</h4>\n\
<select name=\"variables\" multiple size=5 onChange=\"variables_obj.var_selection()\">" << endl;

    for (Pix p = dds.first_var(); p; dds.next_var(p)) {
	_os << "<option value=\"" << dds.var(p)->name() << "\"> "
	    << dds.var(p)->name() << endl;
    }

    _os << "</select>" << endl;
}

void
WWWOutput::write_variable_entries(DAS &das, DDS &dds)
{
    // This writes the text `Variables:' and then sets up the table so that
    // the first variable's section is written into column two.
    _os << \
"<tr>\n\
<td align=\"right\" valign=\"top\">\n\
<h3><a href=\"dods_form_help.html#dataset_variables\">Variables:</a></h3>\n\
<td>";
    
    for (Pix p = dds.first_var(); p; dds.next_var(p)) {
	dds.var(p)->print_val(_os);
	write_variable_attributes(dds.var(p), das);
	_os << "\n<p><p>\n\n";		// End the current var's section
	_os << "<tr><td><td>\n\n";	// Start the next var in column two
    }
}

void
WWWOutput::write_variable_attributes(BaseType *btp, DAS &das)
{
    AttrTable *attr = das.get_table(btp->name());
    // Don't write anything if there are no attributes.
    if (!attr)
	return;

    _os << "<textarea name=\"" << btp->name() << "_attr" << "\" rows="
	<< _attr_rows << " cols=" << _attr_cols << ">\n";
    write_attributes(attr);
    _os << "</textarea>\n\n";
}

string
fancy_typename(BaseType *v)
{
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
	return "string";
      case dods_url_c:
	return "URL";
      case dods_array_c: {
	  ostringstream type;
	  Array *a = (Array *)v;
	  type << "Array of " << fancy_typename(a->var()) <<"s ";
	  for (Pix p = a->first_dim(); p; a->next_dim(p))
	      type << "[" << a->dimension_name(p) << " = 0.." 
		   << a->dimension_size(p, false)-1 << "]";
	  return type.str();
      }
#if 0
      case dods_list_c: {
	  ostringstream type;
	  List *l = (List *)v;
	  type << "List of " << fancy_typename(l->var()) <<"s ";
	  return type.str();
      }
#endif
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
    static Regex global("\\(.*global.*\\)\\|\\(.*dods.*\\)", 1);
    downcase(name);
    return global.match(name.c_str(), name.length()) != -1;
}

void
write_simple_variable(ostream &os, const string &name, const string &type)
{
    os << "<script type=\"text/javascript\">\n"
       << "<!--\n"
       << name_for_js_code(name) <<" = new dods_var(\"" << id2www_ce(name) 
       << "\", \"" 
       << name_for_js_code(name) << "\", 0);\n"
       << "DODS_URL.add_dods_var(" << name_for_js_code(name) << ");\n"
       << "// -->\n"
       << "</script>\n";

    os << "<b>" 
       << "<input type=\"checkbox\" name=\"get_" << name_for_js_code(name) 
       << "\"\n"
       << "onclick=\"" 
       << name_for_js_code(name) << ".handle_projection_change(get_"
       << name_for_js_code(name) << ")\">\n" 
       << "<font size=\"+1\">" << name << "</font>" 
       << ": " << type << "</b><br>\n\n";

    os << name << " <select name=\"" << name_for_js_code(name)<< "_operator\""
       << " onfocus=\"describe_operator()\""
       << " onchange=\"DODS_URL.update_url()\">\n"
       << "<option value=\"=\" selected>=\n"
       << "<option value=\"!=\">!=\n"
       << "<option value=\"<\"><\n"
       << "<option value=\"<=\"><=\n"
       << "<option value=\">\">>\n"
       << "<option value=\">=\">>=\n"
       << "<option value=\"-\">--\n"
       << "</select>\n";

    os << "<input type=\"text\" name=\"" << name_for_js_code(name)
       << "_selection"
       << "\" size=12 onFocus=\"describe_selection()\" "
       << "onChange=\"DODS_URL.update_url()\">\n";
    
    os << "<br>\n\n";
}

// $Log: WWWOutput.cc,v $
// Revision 1.14  2004/01/28 16:47:49  jimg
// Removed case for dods_list_c since List has been removed from the DAP.
// Fixed a missed conflict fromthe last merge.
//
// Revision 1.13  2004/01/26 18:58:48  jimg
// Build fixes
//
// Revision 1.12  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.11  2003/05/08 00:38:20  jimg
// Fixed multi-line string literals.
//
// Revision 1.10.4.1  2003/05/07 22:08:51  jimg
// Added 'using namespace std;', fixed multi-line string literals and replaced
// ostrstream with ostringstream.
//
// Revision 1.10  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.6.2.6  2002/03/27 14:54:45  jimg
// Changed write_attributes so that nested attribtues `work.' The names of
// nested attribtues are printed using the dot notation (cont1.cont2.attr).
//
// Revision 1.6.2.5  2002/02/04 17:11:07  jimg
// Added code that checks for several `file out' tools in the CWD. If found, extra buttons are displayed. These buttons route the URL built in the form through the tool and ultimately return data to the client in the given file type. NB: The DODS server dispatch script (DODS_Dispatch.pm) must be modified also.
//
// Revision 1.9  2001/09/28 23:51:32  jimg
// Merged with 3.2.4.
//
// Revision 1.6.2.4  2001/09/10 21:48:07  jimg
// Removed the `Send to Program' button and its help text.
//
// Revision 1.6.2.3  2001/09/10 19:32:28  jimg
// Fixed two problems: 1) Variable names in the JavaScript code sometimes
// contained spaces since they were made using the dataset's variable name.
// The names are now filtered through id2www and esc2underscore. 2) The CE
// sometimes contained spaces, again, because dataset variable names were
// used to build the CE. I filtered the names with id2www_ce before passing
// them to the JavaScript code.
//
// Revision 1.8  2001/08/27 17:53:07  jimg
// Merged with release-3-2-3.
//
// Revision 1.6.2.2  2001/07/13 20:23:31  jimg
// The string (dods_) used to prefix variable names so that variables which have
// the same name as a JavaScript reserved word was not `unique enough' to avoid
// other conflicts. I changed it to `org_dods_dcz' and added www_int's process
// ID. That should eliminate all conflicts in practice.
//
// Revision 1.7  2001/01/26 19:17:36  jimg
// Merged with release-3-2.
//
// Revision 1.6.2.1  2001/01/26 04:04:33  jimg
// Fixed a bug in the JavaScript code. Now the name of the JS variables
// are prefixed by `dods_'. This means that DODS variables whose names are
// also reserved words in JS work break the JS code.
//
// Revision 1.6  2000/11/09 21:04:37  jimg
// Merged changes from release-3-1. There was a goof and a bunch of the
// changes never made it to the branch. I merged the entire branch.
// There maybe problems still...
//
// Revision 1.5  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
