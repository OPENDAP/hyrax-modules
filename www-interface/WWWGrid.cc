
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

// implementation for WWWGrid. See WWWByte.
//
// 4/7/99 jhrg

#include "config.h"

static char rcsid[] not_used =
    { "$Id$" };

#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Array.h"
#include "escaping.h"
#include "InternalErr.h"

#include "WWWGrid.h"
#include "WWWOutput.h"
#include "get_html_form.h"

using namespace dap_html_form;

BaseType *WWWGrid::ptr_duplicate()
{
    return new WWWGrid(*this);
}

WWWGrid::WWWGrid(const string & n) : Grid(n)
{
}

WWWGrid::WWWGrid(Grid * grid): Grid(grid->name())
{
    BaseType *bt = basetype_to_wwwtype(grid->array_var());
    add_var(bt, array);
    delete bt;

    // To make a valid Grid, this method must set the array dimensions as it
    // adds new maps.
    
    Grid::Map_iter i = grid->map_begin();
    Grid::Map_iter e = grid->map_end();
    while ( i != e ) {
        Array *at = dynamic_cast<Array *>(basetype_to_wwwtype(*i));
        add_var(at, maps);
        delete at;
        ++i;
    }
}


WWWGrid::~WWWGrid()
{
}

void
 WWWGrid::print_val(FILE * os, string space, bool /*print_decl_p */ )
{
    ostringstream ss;
    ss << "<script type=\"text/javascript\">\n"
        << "<!--\n"
        << name_for_js_code(name()) << " = new dods_var(\""
        << id2www_ce(name())
        << "\", \"" << name_for_js_code(name()) << "\", 1);\n"
        << "DODS_URL.add_dods_var(" << name_for_js_code(name()) << ");\n"
        << "// -->\n" << "</script>\n";

    ss << "<b>"
        << "<input type=\"checkbox\" name=\"get_" <<
        name_for_js_code(name())
        << "\"\n" << "onclick=\"" << name_for_js_code(name())
        << ".handle_projection_change(get_"
        << name_for_js_code(name()) << ")\">\n"
        << "<font size=\"+1\">" << name() << "</font>"
        << ": " << fancy_typename(this) << "</b><br>\n\n";

    Array *a = dynamic_cast < Array * >(array_var());
    
    Array::Dim_iter p = a->dim_begin();
    for (int i = 0; p != a->dim_end(); ++i, ++p) {
        int size = a->dimension_size(p, true);
        string n = a->dimension_name(p);
        if (n != "")
            ss << n << ":";
        ss << "<input type=\"text\" name=\"" << name_for_js_code(name())
            << "_" << i
            << "\" size=8 onfocus=\"describe_index()\""
            << "onChange=\"DODS_URL.update_url()\">\n";
        ss << "<script type=\"text/javascript\">\n"
            << "<!--\n"
            << name_for_js_code(name()) << ".add_dim(" << size << ");\n"
            << "// -->\n" << "</script>\n";
    }

    ss << "<br>\n";

    fprintf(os, "%s", ss.str().c_str());
}
