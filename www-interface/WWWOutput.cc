
#include <string>
#include <iostream>
#include <strstream.h>

#include "config_dap.h"
#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "DAS.h"
#include "DDS.h"

#include "WWWOutput.h"

static bool name_is_global(string &name);
static bool name_in_kill_file(const string &name);
static string downcase(const string &s);

WWWOutput::WWWOutput(ostream &os, int rows, int cols):
    _os(os), _attr_rows(rows), _attr_cols(cols)
{
}

void
WWWOutput::write_html_header(bool nph_header)
{
    if (nph_header)
	_os << "HTTP/1.0 200 OK" << endl;
    _os << "XDODS-Server: " << DVR << endl;
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
<td align=\"right\"><h3><a href=\"query-help.html#data_url\">Data URL:</a>
</h3>
<td><input name=\"url\" type=\"text\" size=" << _attr_cols << " value=\"" 
<< url << "\">

<tr>
<td align=\"right\"><h3><a href=\"query-help.html#disposition\">Disposition:</a></h3>
<td><input type=\"button\" value=\"ASCII Data\" onclick=\"ascii_button()\">
<input type=\"button\" value=\"Binary Data\" onclick=\"binary_button()\">
<input type=\"button\" value=\"Data to Matlab\" onclick=\"matlab_button()\">";
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
<a href=\"query-help.html#global_attr\">Global Attributes:</a></h3>
<td><textarea name=\"global_attr\" rows=" << _attr_rows << " cols=" 
<< _attr_cols << ">\n";

    for (Pix p = das.first_var(); p; das.next_var(p)) {
	string name = (const char *)das.get_name(p);

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
       "<a href=\"query-help.html#dataset_variables\"><h4>Dataset Variables</a>:</h4>
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
<h3><a href=\"query-help.html#dataset_variables\">Variables:</a></h3>
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
	return "String";
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
      case dods_function_c:
	return "Function";
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

static string 
downcase(const string &s)
{
    string d("");
    string::const_iterator p = s.begin();
    while (p != s.end())
	d += tolower(*p++);
    return d;
}

static bool
name_is_global(string &name)
{
    static Regex global("\\(.*global.*\\)\\|\\(.*dods.*\\)", 1);
    name = downcase(name);
    return global.match(name.c_str(), name.length()) != -1;
}

void
write_simple_variable(ostream &os, const string &name, const string &type)
{
    os << "<script type=\"text/javascript\">\n"
       << "<!--\n"
       << name << " = new dods_var(\"" << name << "\", 0);\n"
       << "DODS_URL.add_dods_var(" << name << ");\n"
       << "// -->\n"
       << "</script>\n";

    os << "<b>" 
       << "<input type=\"checkbox\" name=\"get_" << name << "\"\n"
       << "onclick=\"" << name << ".handle_projection_change(get_"
       << name << ")\">\n" 
       << "<font size=\"+1\">" << name << "</font>" 
       << ": " << type << "</b><br>\n\n";

    os << name << " <select name=\"" << name << "_operator\""
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

    os << "<input type=\"text\" name=\"" << name << "_selection"
       << "\" size=12 onFocus=\"describe_selection()\" "
       << "onChange=\"DODS_URL.update_url()\">\n";
    
    os << "<br>\n\n";
}
