
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
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

// $Id: www.js,v 1.5 2003/12/08 18:08:02 edavis Exp $

var reflection_cgi = "http://unidata.ucar.edu/cgi-bin/dods/posturl.pl";

// Event handlers for the disposition button.

// The ascii_button handler sends data to a new window. The user can then 
// save the data to a file.

function ascii_button() {
    var url = new String(document.forms[0].url.value);

    var url_parts = url.split("?");
    /* handle case where constraint is null. */
    if (url_parts[1] != null) {
        var ascii_url = url_parts[0] + ".ascii?" + url_parts[1];
    }
    else {
        var ascii_url = url_parts[0] + ".ascii?";
    }

    window.open(ascii_url, "ASCII_Data");
}

/* The binary_button handler loads the data to the current window. Since it 
   is binary, Netscape will ask the user for a filename and save the data
   to that file. */

function binary_button(ext) {
    var url = new String(document.forms[0].url.value);

    var url_parts = url.split("?");
    /* handle case where constraint is null. */
    if (url_parts[1] != null) {
        var binary_url = url_parts[0] + "." + ext + "?" + url_parts[1];
    }
    else {
        var binary_url = url_parts[0] + "." + ext + "?";
    }

    window.location = binary_url;
}

/* Route the URL to Matlab, IDL, .... Users must add an entry into their mime
   types file (aka list of Netscape helper applications) so that the URL will
   be fedd into Matlab which must, in addition, be running loaddods.

   Note that reflection_cgi is a global JavaScript variable set at the 
   begining of this `file'. 

   DEPRECATED */

function program_button() {
    var program_url = new String(document.forms[0].url.value);

    /* Build a call to the reflector CGI. */
    var CGI = reflection_cgi + "?" + "url=" + program_url + "&disposition=matlab";

    window.location = CGI;
}

var help = 0;			// Our friend, the help window.

function help_button() {
    // Check the global to keep from opening the window again if it is
    // already visible. I think Netscape handles this but I know it will
    // write the contents over and over again. This preents that, too.
    // 10/8/99 jhrg
    if (help && !help.closed)
	return;

    // Resize on Netscape 4 is hosed. When enabled, if a user resizes then
    // the root window's document gets reloaded. This does not happen on IE5.
    // regardless, with scrollbars we don't absolutely need to be able to
    // resize. 10/8/99 jhrg
    help = window.open("", "help", "scrollbars,dependent,width=600,height=400");
    write_help_contents(help);
}

function write_help_contents() {
    help.document.writeln("<html><head><title> " +
"Help for the DODS Dataset Access Form</title></head><body><form> " +
"<center><h2>Help for the DODS Dataset Access Form</h2></center> " +
"This form displays information from the dataset whose URL is shown in " +
"the <em>DataURL</em> field. Each variable in this dataset is shown " +
"below in the section labeled <em>Variables</em>. " +
"<ul>" +
"<li>To select a variable click on the checkbox to its left. " +
"<li>To constrain a variable that you've selected, edit the information " +
"that appears in the text boxes below the variable. " +
"When entering values for selections on String variables, you may need " +
"to quote those values. " +
"<li>To get data in ASCII, various common formats or DODS binary objects, click on " +
"one of the buttons labeled <em>Get ...</em>. The <em>Get ASCII</em> and " +
"<em>Get DODS Data Object</em> buttons will always be present. Other buttons " +
"may be present providing access other types of return formats such as NetCDF, " +
"HDF, et cetera. Note that in order to use the DODS Data Objects, you'll need " +
"to download software from the <a href=\"http://www.opendap.org/download/\">OPeNDAP web site</a> " +
"to decode them. " +
//  "<li>To use the <em>Send to Program</em> feature you must install a web " +
//  "helper application that can route the DODS URL built by this form to a " +
//  "client. Look on the <a " +
//  "href=\"http://unidata.ucar.edu/packages/dods/\" target=\"DODS\"> " +
//  "DODS home page (http://unidata.ucar.edu/packages/dods/)</a> " +
//  "for a suitable client. " +
//  "</ul>" +
"Also note that the URL displayed in the <em>DataURL</em> field is updated " +
"as you select and/or constrain variables. The URL in this field can be " +
"cut and pasted in various clients such as the Matlab, ferret, GrADS " +
"et cetera. See the <a " +
"href=\"http://www.opendap.org/\" target=\"DODS\"> " +
"OPeNDAP home page</a> for " +
"information about those clients. " +
"<p><hr><p> " + 
"<center> " +
"<input type=\"button\" value=\"Close\" onclick=\"self.close()\"> " +
"</center></form></body></html>");
}

function open_dods_home() {
    window.open("http://www.opendap.org/", "DODS_HOME_PAGE");
}


// Helper functions for the form.

function describe_index() {
   defaultStatus = "Enter start, stride and stop for the array dimension.";
}

function describe_selection() {
   defaultStatus = "Enter a relational expression (e.g., <20). String variables may need values to be quoted";
}

function describe_operator() {
   defaultStatus = "Choose a relational operator. Use - to enter a function name).";
}

function describe_projection() {
   defaultStatus = "Add this variable to the projection.";
}

///////////////////////////////////////////////////////////
// The dods_url object.
///////////////////////////////////////////////////////////

// CTOR for dods_url
// Create the DODS URL object.
function dods_url(base_url) {
    this.url = base_url;
    this.projection = "";
    this.selection = "";
    this.num_dods_vars = 0;
    this.dods_vars = new Array();
        
    this.build_constraint = build_constraint;
    this.add_dods_var = add_dods_var;
    this.update_url = update_url;
}

// Method of dods_url
// Add the projection and selection to the displayed URL.
function update_url() {
    this.build_constraint();
    var url_text = this.url;
    // Only add the projection & selection (and ?) if there really are
    // constraints! 
    if (this.projection.length + this.selection.length > 0)
        url_text += "?" + this.projection + this.selection;
    document.forms[0].url.value = url_text;
}

// Method of dods_url
// Scan all the form elements and pick out the various pieces of constraint
// information. Add these to the dods_url instance.
function build_constraint() {
    var p = "";
    var s = "";
    for (var i = 0; i < this.num_dods_vars; ++i) {
        if (this.dods_vars[i].is_projected == 1) {
	    // The comma is a clause separator.
	    if (p.length > 0)
	        p += ",";
            p += this.dods_vars[i].get_projection();
	}
	var temp_s = this.dods_vars[i].get_selection();
	if (temp_s.length > 0)
	    s += "&" + temp_s;    // The ampersand is a prefix to the clause.
    }

    this.projection = p;
    this.selection = s;
}

// Method of dods_url
// Add the variable to the dods_var array of dods_vars. The var_index is the
// number of *this particular* variable in the dataset, zero-based.
function add_dods_var(dods_var) {
    this.dods_vars[this.num_dods_vars] = dods_var;
    this.num_dods_vars++;
}

/////////////////////////////////////////////////////////////////
// dods_var
/////////////////////////////////////////////////////////////////

// CTOR for dods_var
// name: the name of the variable from DODS' perspective.
// js_var_name: the name of the variable within the form.
// is_array: 1 if this is an array, 0 otherwise.
function dods_var(name, js_var_name, is_array) {
    // Common members
    this.name = name;
    this.js_var_name = js_var_name;
    this.is_projected = 0;
    if (is_array > 0) {
        this.is_array = 1;
        this.num_dims = 0;        // Holds the number of dimensions
        this.dims = new Array(); // Holds the length of the dimensions

        this.add_dim = add_dim;
        this.display_indices = display_indices;
        this.erase_indices = erase_indices;
    }
    else
        this.is_array = 0;

    this.handle_projection_change = handle_projection_change;
    this.get_projection = get_projection;
    this.get_selection = get_selection;
}

// Method of dods_var
// Add a dimension to a DODS Array object.
function add_dim(dim_size) {
    this.dims[this.num_dims] = dim_size;
    this.num_dims++;
}

// Method of dods_var
// Add the array indices to the text widgets associated with this DODS
// array object. The text widgets are names <var_name>_0, <var_name>_1, ...
// <var_name>_n for an array with size N+1.
function display_indices() {
    for (var i = 0; i < this.num_dims; ++i) {
        var end_index = this.dims[i]-1;
        var s = "0:1:" + end_index.toString();
	var text_widget = "document.forms[0]." + this.js_var_name + "_" + i.toString();
	eval(text_widget).value = s;
    }
}

// Method of dods_var
// Use this to remove index information from a DODS array object.
function erase_indices() {
    for (var i = 0; i < this.num_dims; ++i) {
	var text_widget = "document.forms[0]." + this.js_var_name + "_" + i.toString();
	eval(text_widget).value = "";
    }
}

// Method of  dods_var
function handle_projection_change(check_box) {
    if (check_box.checked) {
        this.is_projected = 1;
	if (this.is_array == 1)
	    this.display_indices();
    }
    else {
        this.is_projected = 0;
	if (this.is_array == 1)
	    this.erase_indices();
    }

    DODS_URL.update_url();
}


// Method of dods_var
// Get the projection sub-expression for this variable.
function get_projection() {
    var p = "";
    if (this.is_array == 1) {
        p = this.name;		// ***
        for (var i = 0; i < this.num_dims; ++i) {
	    var text_widget = "document.forms[0]." + this.js_var_name + "_" + i.toString();
	    p += "[" + eval(text_widget).value + "]";
	}
    }
    else {
	p = this.name;		// ***
    }

    return p;
}

// Method of dods_var
// Get the selection (which is null for arrays).
function get_selection() {
    var s = "";
    if (this.is_array == 1) {
        return s;
    }
    else {
	var text_widget = "document.forms[0]." + this.js_var_name + "_selection";
        if (eval(text_widget).value != "") {
            var oper_widget_name = "document.forms[0]." + this.js_var_name + "_operator";
            var oper_widget = eval(oper_widget_name);
	    var operator = oper_widget.options[oper_widget.selectedIndex].value;
            // If the operator is `-' then don't prepend the variable name!
            // This provides a way for users to enter function names as
            // selection clauses. 
            if (operator == "-")
                s = eval(text_widget).value;
            else
	        s = this.name + operator + eval(text_widget).value; // ***
        }
    }

    return s;
}    
