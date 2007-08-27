
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

#include "config.h"

#include <iostream>
#include <string>

using namespace std;

#include "InternalErr.h"

// #define DODS_DEBUG

#include "AsciiGrid.h"
#include "AsciiArray.h"
#include "debug.h"
#include "get_ascii.h"

using namespace dap_asciival;

BaseType *
AsciiGrid::ptr_duplicate()
{
    return new AsciiGrid(*this);
}

AsciiGrid::AsciiGrid(const string &n) : Grid(n)
{
}

AsciiGrid::AsciiGrid(Grid * grid):AsciiOutput(grid)
{
    BaseType *bt = basetype_to_asciitype(grid->array_var());
    add_var(bt, array);
    // add_var makes a copy of the base type passed to it, so delete it here
    delete bt;
    bt = 0;

    Grid::Map_iter i = grid->map_begin();
    Grid::Map_iter e = grid->map_end();
    while ( i != e ) {
        bt = basetype_to_asciitype(*i);
        add_var(bt, maps);
        // add_var makes a copy of the base type passed to it, so delete it here
        delete bt;
        ++i;
    }
    set_name(grid->name());
}

AsciiGrid::~AsciiGrid()
{
}

void
AsciiGrid::print_ascii(FILE * os, bool print_name) throw(InternalErr)
{
    Grid *g = dynamic_cast < Grid * >(_redirect);
    if (!g)
        g = this;

    if (dynamic_cast < Array * >(g->array_var())->dimensions(true) > 1)
        print_grid(os, print_name);
    else
        print_vector(os, print_name);
}

void
AsciiGrid::print_ascii(ostream &strm, bool print_name) throw(InternalErr)
{
    Grid *g = dynamic_cast < Grid * >(_redirect);
    if (!g)
        g = this;

    if (dynamic_cast < Array * >(g->array_var())->dimensions(true) > 1)
        print_grid(strm, print_name);
    else
        print_vector(strm, print_name);
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

// Similar to AsciiArray's print_vector. Print a Grid that has only one
// dimension. To fit the spec we can call print_ascii() on the map vector and
// then the array (which has only one dimension). This is a special case; if
// a grid has two or more dimensions then we can't use the AsciiArray code.
void
AsciiGrid::print_vector(ostream &strm, bool print_name)
{
    DBG(cerr << "AsciiGrid::print_vector" << endl);

    dynamic_cast<AsciiArray&>(**map_begin()).print_ascii(strm, print_name);
    
    strm << "\n" ;

    dynamic_cast<AsciiArray&>(*array_var()).print_ascii(strm, print_name);
}

void
AsciiGrid::print_grid(FILE * os, bool print_name)
{
    DBG(cerr << "AsciiGrid::print_grid" << endl);

    Grid *g = dynamic_cast < Grid * >(_redirect);
    if (!g) {
        g = this;
    }
    // Grab the Grid's array
    Array *grid_array = dynamic_cast < Array * >(g->array_var());
    AsciiArray *a_grid_array = dynamic_cast < AsciiArray * >(array_var());
    AsciiOutput *ao_grid_array =
        dynamic_cast < AsciiOutput * >(a_grid_array);

    // Set up the shape and state vectors. Shape holds the shape of this
    // array, state holds the index of the current vector to print.
    int dims = grid_array->dimensions(true);
    if (dims <= 1)
        throw InternalErr(__FILE__, __LINE__,
                          "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of each dimension of the array
    // (not the size; each value is one less that the size). 
    vector < int >shape = a_grid_array->get_shape_vector(dims - 1);
    int rightmost_dim_size = a_grid_array->get_nth_dim_size(dims - 1);

    // state holds the indexes of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row.
    vector < int >state(dims - 1, 0);

    // Now that we have the number of dims, get and print the rightmost map.
    // This is cumbersome; if we used the STL it would be much less so.
    // We are now using STL, so it isn't so cumbersome. pcw
    {
        // By definition, a map is a vector. Print the rightmost map.
        dynamic_cast < AsciiArray & >(**(map_begin() + dims - 1))
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
        string n = ao_grid_array->get_full_name();

        fprintf(os, "%s", n.c_str());

        vector < int >::iterator state_iter = state.begin();
        Grid::Map_iter p = g->map_begin();
        Grid::Map_iter ap = map_begin();
        while (state_iter != state.end()) {
            Array *map = dynamic_cast < Array * >(*p);
            AsciiArray *amap = dynamic_cast < AsciiArray * >(*ap);
            AsciiOutput *aomap = dynamic_cast < AsciiOutput * >(amap);

            fprintf(os, "[%s=", aomap->get_full_name().c_str());
            BaseType *avar = basetype_to_asciitype(map->var(*state_iter));
            AsciiOutput *aovar = dynamic_cast < AsciiOutput * >(avar);
            aovar->print_ascii(os, false);
            // we aren't saving avar for future reference so need to delete
            delete avar;
            fprintf(os, "]");

            state_iter++;
            p++;
            ap++;
        }
        fprintf(os, ", ");;

        index = a_grid_array->print_row(os, index, rightmost_dim_size - 1);

        more_indices = increment_state(&state, shape);
        if (more_indices)
            fprintf(os, "\n");

    } while (more_indices);
}

void
AsciiGrid::print_grid(ostream &strm, bool print_name)
{
    DBG(cerr << "AsciiGrid::print_grid" << endl);

    Grid *g = dynamic_cast < Grid * >(_redirect);
    if (!g) {
        g = this;
    }
    // Grab the Grid's array
    Array *grid_array = dynamic_cast < Array * >(g->array_var());
    AsciiArray *a_grid_array = dynamic_cast < AsciiArray * >(array_var());
    AsciiOutput *ao_grid_array =
        dynamic_cast < AsciiOutput * >(a_grid_array);

    // Set up the shape and state vectors. Shape holds the shape of this
    // array, state holds the index of the current vector to print.
    int dims = grid_array->dimensions(true);
    if (dims <= 1)
        throw InternalErr(__FILE__, __LINE__,
                          "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of each dimension of the array
    // (not the size; each value is one less that the size). 
    vector < int >shape = a_grid_array->get_shape_vector(dims - 1);
    int rightmost_dim_size = a_grid_array->get_nth_dim_size(dims - 1);

    // state holds the indexes of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row.
    vector < int >state(dims - 1, 0);

    // Now that we have the number of dims, get and print the rightmost map.
    // This is cumbersome; if we used the STL it would be much less so.
    // We are now using STL, so it isn't so cumbersome. pcw
    {
        // By definition, a map is a vector. Print the rightmost map.
        dynamic_cast < AsciiArray & >(**(map_begin() + dims - 1))
            .print_ascii(strm, print_name);
        strm << "\n" ;
    }

    bool more_indices;
    int index = 0;
    do {
        // Print indices for all dimensions except the last one. Include the
        // name of the corresponding map vector and the *value* of this
        // index. Note that the successive elements of state give the indeces
        // of each of the N-1 dimensions for the current row.
        string n = ao_grid_array->get_full_name();

        strm << n ;

        vector < int >::iterator state_iter = state.begin();
        Grid::Map_iter p = g->map_begin();
        Grid::Map_iter ap = map_begin();
        while (state_iter != state.end()) {
            Array *map = dynamic_cast < Array * >(*p);
            AsciiArray *amap = dynamic_cast < AsciiArray * >(*ap);
            AsciiOutput *aomap = dynamic_cast < AsciiOutput * >(amap);

            strm << "[" << aomap->get_full_name() << "=" ;
            BaseType *avar = basetype_to_asciitype(map->var(*state_iter));
            AsciiOutput *aovar = dynamic_cast < AsciiOutput * >(avar);
            aovar->print_ascii(strm, false);
            // we aren't saving avar for future reference so need to delete
            delete avar;
            strm << "]" ;

            state_iter++;
            p++;
            ap++;
        }
        strm << ", " ;

        index = a_grid_array->print_row(strm, index, rightmost_dim_size - 1);

        more_indices = increment_state(&state, shape);
        if (more_indices)
            strm << "\n" ;

    } while (more_indices);
}

