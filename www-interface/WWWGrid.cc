
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for WWWGrid. See WWWByte.
//
// 4/7/99 jhrg

// $Log: WWWGrid.cc,v $
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream>
#include <string>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "Array.h"
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
WWWGrid::read(const string &, int &)
{
    assert(false);
    return false;
}

void
WWWGrid::print_val(ostream &os, string space, bool print_decl_p)
{
    os << "<script type=\"text/javascript\">\n"
       << "<!--\n"
       << name() << " = new dods_var(\"" << name() << "\", 1);\n"
       << "DODS_URL.add_dods_var(" << name() << ");\n"
       << "// -->\n"
       << "</script>\n";

    os << "<b>" 
       << "<input type=\"checkbox\" name=\"get_" << name() << "\"\n"
       << "onclick=\"" << name() << ".handle_projection_change(get_"
       << name() << ")\">\n" 
       << "<font size=\"+1\">" << name() << "</font>"
       << ": " << fancy_typename(this) << "</b><br>\n\n";

    Array *a = dynamic_cast<Array *>(array_var());
#if 0
    if (!a)
	throw Error(unknown_error, "Expected an Array\n");
#endif

    Pix p = a->first_dim();
    for (int i = 0; p; ++i, a->next_dim(p)) {
	int size = a->dimension_size(p, true);
	string n = a->dimension_name(p);
	if (n != "")
	    os << n << ":";
	os << "<input type=\"text\" name=\"" << name() << "_" << i 
	   << "\" size=8 onfocus=\"describe_index()\""
	   << "onChange=\"DODS_URL.update_url()\">\n";
	os << "<script type=\"text/javascript\">\n"
	   << "<!--\n"
	   << name() << ".add_dim(" << size << ");\n"
	   << "// -->\n"
	   << "</script>\n";
    }
    
    os << "<br>\n";
}
