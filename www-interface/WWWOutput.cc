
// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWOutput.cc,v 1.7 2001/01/26 19:17:36 jimg Exp $"};

#include <string>
#include <iostream>
#include <strstream.h>

#include "Regex.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "DAS.h"
#include "DDS.h"
#include "InternalErr.h"
#include "util.h"

#include "WWWOutput.h"

static bool name_is_global(string &name);
static bool name_in_kill_file(const string &name);
#if 0
static string downcase(const string &s);
#endif

string
name_for_js_code(const string &dods_name)
{
    return "dods_" + dods_name;
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

    _os << \
"<tr>
<td align=\"right\"><h3><a href=\"dods_form_help.html#disposition\" valign=\"bottom\">Action:</a></h3>
<td><input type=\"button\" value=\"Get ASCII\" onclick=\"ascii_button()\">
<input type=\"button\" value=\"Get Binary\" onclick=\"binary_button()\">
<input type=\"button\" value=\"Send to Program\" onclick=\"program_button()\">
<input type=\"button\" value=\"Show Help\" onclick=\"help_button()\">

<tr>
<td align=\"right\"><h3><a href=\"dods_form_help.html#data_url\" valign=\"bottom\">Data URL:</a>
</h3>
<td><input name=\"url\" type=\"text\" size=" << _attr_cols << " value=\"" 
<< url << "\">"; 
}

void
WWWOutput::write_attributes(AttrTable *attr)
{
    if (attr) {
	for (Pix a = attr->first_attr(); a; attr->next_attr(a)) {
	    int num_attr = attr->get_attr_num(a);

	    _os << attr->get_name(a) << ": ";
	    for (int i = 0; i < num_attr; 
		 ++i, (void)(i<num_attr && _os << ", "))
		_os << attr->get_attr(a, i);
	    _os << endl;
	}
    }
}

void
WWWOutput::write_global_attributes(DAS &das)
{
    _os << \
"
<tr>
<td align=\"right\" valign=\"top\"><h3>
<a href=\"dods_form_help.html#global_attr\">Global Attributes:</a></h3>
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
       "<a href=\"dods_form_help.html#dataset_variables\"><h4>Dataset Variables</a>:</h4>
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
"
<tr>
<td align=\"right\" valign=\"top\">
<h3><a href=\"dods_form_help.html#dataset_variables\">Variables:</a></h3>
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
	  ostrstream type;
	  Array *a = (Array *)v;
	  type << "Array of " << fancy_typename(a->var()) <<"s ";
	  for (Pix p = a->first_dim(); p; a->next_dim(p))
	      type << "[" << a->dimension_name(p) << " = 0.." 
		   << a->dimension_size(p, false)-1 << "]";
	  type << ends;
	  string fancy = type.str();
	  type.freeze(0);
	  return fancy;
      }
      case dods_list_c: {
	  ostrstream type;
	  List *l = (List *)v;
	  type << "List of " << fancy_typename(l->var()) <<"s " << ends;
	  string fancy = type.str();
	  type.freeze(0);
	  return fancy;
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

// This code could use a real `kill-file' some day - about the same time that
// the rest of the server gets a `rc' file... For the present just see if a
// small collection of regexs match the name.

static bool
name_in_kill_file(const string &name)
{
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dimension' attributes.

    return dim.match(name.c_str(), name.length()) != -1;
}

#if 0
static string 
downcase(const string &s)
{
    string d("");
    string::const_iterator p = s.begin();
    while (p != s.end())
	d += tolower(*p++);
    return d;
}
#endif

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
       << name_for_js_code(name) <<" = new dods_var(\"" << name << "\", \"" 
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
