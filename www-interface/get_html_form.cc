
// -*- mode: c++; c-basic-offset:4 -*-

// Copyright (c) 2006 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
// more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// This file holds the interface for the 'get data as ascii' function of the
// OPeNDAP/HAO data server. This function is called by the BES when it loads
// this as a module. The functions in the file ascii_val.cc also use this, so
// the same basic processing software can be used both by Hyrax and tie older
// Server3.

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include <DataDDS.h>
#include <escaping.h>

#include "get_html_form.h"
#include "WWWOutput.h"
#include "WWWOutputFactory.h"

#include "WWWByte.h"
#include "WWWInt16.h"
#include "WWWUInt16.h"
#include "WWWInt32.h"
#include "WWWUInt32.h"
#include "WWWFloat32.h"
#include "WWWFloat64.h"
#include "WWWStr.h"
#include "WWWUrl.h"
#include "WWWArray.h"
#include "WWWStructure.h"
#include "WWWSequence.h"
#include "WWWGrid.h"

namespace dap_html_form {

#include "javascript.h"         // Try to hide this stuff...

// A better way to do this would have been to make WWWStructure, ..., inherit
// from both Structure and WWWOutput. But I didn't... jhrg 8/29/05
WWWOutput *wo = 0;

/** Given a BaseType varaible, return a pointer to a new variable that has the
    same parent types (Byte, et c.) but is now one of the WWW specializations.
    
    @param bt A BaseType such as NCArray, HDFArray, ...
    @return A BaseType that uses the WWW specialization (e.g., WWWArray). */
BaseType *basetype_to_wwwtype(BaseType * bt)
{
    switch (bt->type()) {
    case dods_byte_c:
        return new WWWByte(dynamic_cast < Byte * >(bt));
    case dods_int16_c:
        return new WWWInt16(dynamic_cast < Int16 * >(bt));
    case dods_uint16_c:
        return new WWWUInt16(dynamic_cast < UInt16 * >(bt));
    case dods_int32_c:
        return new WWWInt32(dynamic_cast < Int32 * >(bt));
    case dods_uint32_c:
        return new WWWUInt32(dynamic_cast < UInt32 * >(bt));
    case dods_float32_c:
        return new WWWFloat32(dynamic_cast < Float32 * >(bt));
    case dods_float64_c:
        return new WWWFloat64(dynamic_cast < Float64 * >(bt));
    case dods_str_c:
        return new WWWStr(dynamic_cast < Str * >(bt));
    case dods_url_c:
        return new WWWUrl(dynamic_cast < Url * >(bt));
    case dods_array_c:
        return new WWWArray(dynamic_cast < Array * >(bt));
    case dods_structure_c:
        return new WWWStructure(dynamic_cast < Structure * >(bt));
    case dods_sequence_c:
        return new WWWSequence(dynamic_cast < Sequence * >(bt));
    case dods_grid_c:
        return new WWWGrid(dynamic_cast < Grid * >(bt));
    }
}

/** Given a DDS filled with variables that are one specialization of BaseType,
    build a second DDS which contains variables made using the WWW 
    specialization.
    
    @param dds A DDS
    @return A DDS where each variable in \e dds is now a WWW* variable. */
DDS *dds_to_www_dds(DDS * dds)
{
    // Should the following use WWWOutputFactory instead of the source DDS'
    // factory class?
    DDS *wwwdds = new DDS(dds->get_factory(), dds->get_dataset_name());

    DDS::Vars_iter i = dds->var_begin();
    while (i != dds->var_end()) {
        BaseType *abt = basetype_to_wwwtype(*i);
        wwwdds->add_var(abt);
        // add_var makes a copy of the base type passed to it, so delete it
        // here
        delete abt;
        i++;
    }

    return wwwdds;
}

/** Using the stuff in WWWOutput and the hacked (specialized) print_val() 
    methods in the WWW* classes, write out the HTML form interface.
    
    @todo Modify this code to expect a DDS whose BaseTypes are loaded with
    Attribute tables.
    
    @param dest Write HTML here.
    @param dds A DataDDS loaded with data; the BaseTypes in this must be
    WWW* instances,
    @param url The URL that should be initially displayed in the form. The
    form's javescript code will update this incrementally as the user builds
    a constraint.
    @param html_header Print a HTML header/footer for the page. True by default.
    @param admin_name "support@opendap.org" by default; use this as the 
    address for support printed on the form.
    @param help_location "http://www.opendap.org/online_help_files/opendap_form_help.html" 
    by default; otherwise, this is locataion where an HTML document that 
    explains the page can be found. 
    */
void write_html_form_interface(FILE * dest, DDS * dds,
                               const string & url, bool html_header,
                               const string & admin_name,
                               const string & help_location)
{
    wo = new WWWOutput(dest);

    if (html_header)
        wo->write_html_header();

    ostringstream oss;
    oss <<
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
        << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n" <<
        "<html><head><title>OPeNDAP Server Dataset Query Form</title>\n"
        << "<base href=\"" << help_location << "\">\n" <<
        "<script type=\"text/javascript\">\n" << "<!--\n"
        // Javascript code here
        << java_code << "\n"
        << "DODS_URL = new dods_url(\"" << url << "\");\n"
        << "// -->\n"
        << "</script>\n"
        << "</head>\n"
        << "<body>\n"
        <<
        "<p><h2 align='center'>OPeNDAP Server Dataset Access Form</h2>\n"
        << "<hr>\n" << "<form action=\"\">\n" << "<table>\n";
    fprintf(stdout, "%s", oss.str().c_str());

    wo->write_disposition(url);

    fprintf(stdout, "<tr><td><td><hr>\n\n");

    wo->write_global_attributes(dds->get_attr_table());

    fprintf(stdout, "<tr><td><td><hr>\n\n");

    wo->write_variable_entries(*dds);

    oss.str("");
    oss << "</table></form>\n\n" << "<hr>\n\n";
    oss << "<address>Send questions or comments to: <a href=\"mailto:"
        << admin_name << "\">" << admin_name << "</a></address>" << "<p>\n\
                    <a href=\"http://validator.w3.org/check?uri=referer\"><img\n\
                        src=\"http://www.w3.org/Icons/valid-html40\"\n\
                        alt=\"Valid HTML 4.0 Transitional\" height=\"31\" width=\"88\">\n\
                    </a></p>\n" << "</body></html>\n";

    fprintf(stdout, "%s", oss.str().c_str());

}

const string allowable =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

// This function adds some text to the variable name so that conflicts with
// JavaScript's reserved words and other conflicts are avoided (or
// minimized...) 7/13/2001 jhrg
//
// I've modified this so that it no longer include the PID. Not sure why that
// was included... However, removing it will make using the HTML form in tests
// easier. jhrg 11/28/06
string name_for_js_code(const string & dods_name)
{
    return string("org_opendap_") +
        esc2underscore(id2www(dods_name, allowable));
}

string fancy_typename(BaseType * v)
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
    case dods_array_c:{
            ostringstream type;
            Array *a = (Array *) v;
            type << "Array of " << fancy_typename(a->var()) << "s ";
            for (Array::Dim_iter p = a->dim_begin(); p != a->dim_end();
                 ++p)
                type << "[" << a->dimension_name(p) << " = 0.." << a->
                    dimension_size(p, false) - 1 << "]";
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

/** This function is used by the Byte, ..., URL types to write their entries
    in the HTML Form. More complex classes do something else.
    @brief Output HTML entry for Scalars
    @param os Write to this output sink.
    @param name The name of the variable.
    @param type The name of the variable's data type. */
void
write_simple_variable(FILE * os, const string & name, const string & type)
{
    ostringstream ss;
    ss << "<script type=\"text/javascript\">\n"
        << "<!--\n"
        << name_for_js_code(name) << " = new dods_var(\"" <<
        id2www_ce(name)
        << "\", \"" << name_for_js_code(name) << "\", 0);\n" <<
        "DODS_URL.add_dods_var(" << name_for_js_code(name) << ");\n" <<
        "// -->\n" << "</script>\n";

    ss << "<b>"
        << "<input type=\"checkbox\" name=\"get_" << name_for_js_code(name)
        << "\"\n" << "onclick=\"" << name_for_js_code(name) <<
        ".handle_projection_change(get_" << name_for_js_code(name) <<
        ") \"  onfocus=\"describe_projection()\">\n" << "<font size=\"+1\">" 
        << name << "</font>" << ": "
        << type << "</b><br>\n\n";

    ss << name << " <select name=\"" << name_for_js_code(name) <<
        "_operator\"" << " onfocus=\"describe_operator()\"" <<
        " onchange=\"DODS_URL.update_url()\">\n" <<
        "<option value=\"=\" selected>=\n" <<
        "<option value=\"!=\">!=\n" << "<option value=\"<\"><\n" <<
        "<option value=\"<=\"><=\n" << "<option value=\">\">>\n" <<
        "<option value=\">=\">>=\n" << "<option value=\"-\">--\n" <<
        "</select>\n";

    ss << "<input type=\"text\" name=\"" << name_for_js_code(name)
        << "_selection"
        << "\" size=12 onFocus=\"describe_selection()\" "
        << "onChange=\"DODS_URL.update_url()\">\n";

    ss << "<br>\n\n";

    // Now write that string to os
    fprintf(os, "%s", ss.str().c_str());
}

} // namespace dap_html_form
