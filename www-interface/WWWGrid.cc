
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for WWWGrid. See WWWByte.
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWGrid.cc,v 1.6 2001/09/28 23:51:32 jimg Exp $"};

#include <assert.h>
#include <iostream>
#include <string>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "Array.h"
#include "escaping.h"
#include "InternalErr.h"

#include "WWWGrid.h"
#include "WWWOutput.h"

Grid *
NewGrid(const string &n)
{
    return new WWWGrid(n);
}

BaseType *
WWWGrid::ptr_duplicate()
{
    return new WWWGrid(*this);
}

WWWGrid::WWWGrid(const string &n) : Grid(n)
{
}

WWWGrid::~WWWGrid()
{
}

bool
WWWGrid::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void
WWWGrid::print_val(ostream &os, string space, bool print_decl_p)
{
    os << "<script type=\"text/javascript\">\n"
       << "<!--\n"
       << name_for_js_code(name()) << " = new dods_var(\"" 
       << id2www_ce(name()) 
       << "\", \"" << name_for_js_code(name()) << "\", 1);\n"
       << "DODS_URL.add_dods_var(" << name_for_js_code(name()) << ");\n"
       << "// -->\n"
       << "</script>\n";

    os << "<b>" 
       << "<input type=\"checkbox\" name=\"get_" << name_for_js_code(name())
       << "\"\n"
       << "onclick=\"" << name_for_js_code(name()) 
       << ".handle_projection_change(get_"
       << name_for_js_code(name()) << ")\">\n" 
       << "<font size=\"+1\">" << name() << "</font>"
       << ": " << fancy_typename(this) << "</b><br>\n\n";

    Array *a = dynamic_cast<Array *>(array_var());

    Pix p = a->first_dim();
    for (int i = 0; p; ++i, a->next_dim(p)) {
	int size = a->dimension_size(p, true);
	string n = a->dimension_name(p);
	if (n != "")
	    os << n << ":";
	os << "<input type=\"text\" name=\"" << name_for_js_code(name())
	   << "_" << i 
	   << "\" size=8 onfocus=\"describe_index()\""
	   << "onChange=\"DODS_URL.update_url()\">\n";
	os << "<script type=\"text/javascript\">\n"
	   << "<!--\n"
	   << name_for_js_code(name()) << ".add_dim(" << size << ");\n"
	   << "// -->\n"
	   << "</script>\n";
    }
    
    os << "<br>\n";
}

// $Log: WWWGrid.cc,v $
// Revision 1.6  2001/09/28 23:51:32  jimg
// Merged with 3.2.4.
//
// Revision 1.4.2.2  2001/09/10 19:32:28  jimg
// Fixed two problems: 1) Variable names in the JavaScript code sometimes
// contained spaces since they were made using the dataset's variable name.
// The names are now filtered through id2www and esc2underscore. 2) The CE
// sometimes contained spaces, again, because dataset variable names were
// used to build the CE. I filtered the names with id2www_ce before passing
// them to the JavaScript code.
//
// Revision 1.5  2001/01/26 19:17:36  jimg
// Merged with release-3-2.
//
// Revision 1.4.2.1  2001/01/26 04:04:33  jimg
// Fixed a bug in the JavaScript code. Now the name of the JS variables
// are prefixed by `dods_'. This means that DODS variables whose names are
// also reserved words in JS work break the JS code.
//
// Revision 1.4  2000/10/03 20:07:20  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//
