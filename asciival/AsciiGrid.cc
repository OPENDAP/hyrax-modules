
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an ASCII
// representation of the data read from a DAP server.

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
 
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// implementation for AsciiGrid. See AsciiByte.
//
// 3/12/98 jhrg

#include "config_asciival.h"

#include <iostream>
#include <string>

using namespace std;

#include "InternalErr.h"

// #define DODS_DEBUG

#include "AsciiGrid.h"
#include "AsciiArray.h"
#include "name_map.h"
#include "debug.h"

extern bool translate;
extern name_map names;

BaseType *
AsciiGrid::ptr_duplicate()
{
    return new AsciiGrid(*this);
}

AsciiGrid::AsciiGrid(const string &n) : Grid(n)
{
}

AsciiGrid::~AsciiGrid()
{
}

bool
AsciiGrid::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void
AsciiGrid::print_ascii(FILE *os, bool print_name) throw(InternalErr)
{
    if (dynamic_cast<AsciiArray *>(array_var())->dimensions(true) > 1)
	print_grid(os, print_name);
    else
	print_vector(os, print_name);
}

// Similar to AsciiArray's print_vector. Print a Grid that has only one
// dimension. To fit the spec we can call print_ascii() on the map vector and
// then the array (which has only one dimension). This is a special case; if
// a grid has two or more dimensions then we can't use the AsciiArray code.
void
AsciiGrid::print_vector(FILE *os, bool print_name)
{
    DBG(cerr << "AsciiGrid::print_vector" << endl);

    dynamic_cast<AsciiArray&>(**map_begin()).print_ascii(os, print_name);
    
    fprintf(os, "\n");

    dynamic_cast<AsciiArray&>(*array_var()).print_ascii(os, print_name);
}

void 
AsciiGrid::print_grid(FILE *os, bool print_name)
{
    DBG(cerr << "AsciiGrid::print_grid" << endl);

    // Grab the Grid's array
    AsciiArray *grid_array = dynamic_cast<AsciiArray *>(array_var());

    // Set up the shape and state vectors. Shape holds the shape of this
    // array, state holds the index of the current vector to print.
    int dims = grid_array->dimensions(true);
    if (dims <= 1)
	throw InternalErr(__FILE__, __LINE__, 
	  "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of each dimension of the array
    // (not the size; each value is one less that the size). 
    vector<int> shape = grid_array->get_shape_vector(dims - 1);
    int rightmost_dim_size = grid_array->get_nth_dim_size(dims - 1);

    // state holds the indexes of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row.
    vector<int> state(dims - 1, 0);

    // Now that we have the number of dims, get and print the rightmost map.
    // This is cumbersome; if we used the STL it would be much less so.
    {
	// By definition, a map is a vector. Print the rightmost map.
	dynamic_cast<AsciiArray &>(**(map_begin() + dims - 1))
	    .print_ascii(os, print_name);
	fprintf(os, "\n");
    }

    bool more_indices;
    int index = 0;
    do {
	// Print indices for all dimensions except the last one. Include the
	// name of the corresponding map vector and the *value* of this
	// index. Note that the successive elements of state give the indeces
	// of each of the N-1 dimensions for the current row.
	fprintf(os, "%s",
                names.lookup(dynamic_cast<AsciiOutput*>(grid_array)->get_full_name(), translate).c_str());
	vector<int>::iterator state_iter = state.begin();
	Grid::Map_iter p = map_begin();
	while (state_iter != state.end()) {
	    fprintf(os, "[%s=", 
                dynamic_cast<AsciiOutput &>(**p).get_full_name().c_str());
	    AsciiArray &m = dynamic_cast<AsciiArray &>(**p);
	    dynamic_cast<AsciiOutput &>(*m.var(*state_iter))
		.print_ascii(os, false);
	    fprintf(os, "]");

	    state_iter++; p++;
	}
	fprintf(os, ", ");;

	index = grid_array->print_row(os, index, rightmost_dim_size - 1);

	more_indices = increment_state(&state, shape);
	if (more_indices)
	    fprintf(os, "\n");

    } while (more_indices);
}

// $Log: AsciiGrid.cc,v $
// Revision 1.6  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.4.4.2  2002/12/18 23:41:25  pwest
// gcc3.2 compile corrections, mainly regarding using statements
//
// Revision 1.5  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.4.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:53  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:16  jimg
// Added
//
