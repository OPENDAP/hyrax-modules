
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

// Implementation for AsciiArray. See AsciiByte.cc
//
// 3/12/98 jhrg

#include <iostream>

using std::cerr ;
using std::endl ;

#include "config.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// #define DODS_DEBUG

#include "InternalErr.h"
#include "debug.h"

#include "AsciiArray.h"
#include "util.h"
#include "name_map.h"
#include "get_ascii.h"

using namespace dap_asciival;

BaseType *
AsciiArray::ptr_duplicate()
{
    return new AsciiArray(*this);
}

AsciiArray::AsciiArray(const string &n, BaseType *v) : Array(n, v)
{
}

AsciiArray::AsciiArray( Array *bt ) : AsciiOutput( bt )
{
    // By calling var() without any parameters we get back the template
    // itself, then we can add it to this Array as the template. By doing
    // this we set the parent as well, which is what we need.
    add_var( basetype_to_asciitype( bt->var() ) ) ;
    set_name( bt->name() ) ;
}

AsciiArray::~AsciiArray()
{
}

void
AsciiArray::print_ascii(FILE *os, bool print_name) throw(InternalErr)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
    {
	bt = this ;
    }
    if (bt->var()->is_simple_type())
    {
	if (bt->dimensions(true) > 1)
	{
	    print_array(os, print_name);
	}
	else
	{
	    print_vector(os, print_name);
	}
    }
    else
    {
	print_complex_array(os, print_name);
    }
}

// Print out a values for a vector (one dimensional array) of simple types.
void
AsciiArray::print_vector(FILE *os, bool print_name)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
    {
	bt = this ;
    }

    if (print_name)
	fprintf(os, "%s, ",
                names->lookup(dynamic_cast<AsciiOutput*>(this)->get_full_name(), translate).c_str());

    // only one dimension
    int end = bt->dimension_size(bt->dim_begin(), true) - 1;

    for (int i = 0; i < end; ++i)
    {
	BaseType *curr_var = basetype_to_asciitype( bt->var(i) ) ;
	dynamic_cast<AsciiOutput *>(curr_var)->print_ascii(os, false);
	fprintf(os, ", ");
    }
    BaseType *curr_var = basetype_to_asciitype( bt->var(end) ) ;
    dynamic_cast<AsciiOutput *>(curr_var)->print_ascii(os, false);
}

/** Print a single row of values for a N-dimensional array. Since we store
    N-dim arrays in vectors, #index# gives the starting point in that vector
    for this row and #number# is the number of values to print. The counter
    #index# is returned. 

    @param os Write to stream os.
    @param index Print values starting from this point.
    @param number Print this many values.
    @return One past the last value printed (i.e., the index of the next
    row's first value).
    @see print\_array */
int
AsciiArray::print_row(FILE *os, int index, int number)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
    {
	bt = this ;
    }

    for (int i = 0; i < number; ++i)
    {
	BaseType *curr_var = basetype_to_asciitype( bt->var(index++) ) ;
	dynamic_cast<AsciiOutput *>(curr_var)->print_ascii(os, false);
        fprintf(os, ", ");
    }
    BaseType *curr_var = basetype_to_asciitype( bt->var(index++) ) ;
    dynamic_cast<AsciiOutput *>(curr_var)->print_ascii(os, false);

    return index;
}

// Given a vector of indices, return the cooresponding index.

int
AsciiArray::get_index(vector<int> indices) throw(InternalErr)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
	bt = this ;

    if (indices.size() != bt->dimensions(true)) {
	throw InternalErr(__FILE__, __LINE__, 
			  "Index vector is the wrong size!");
    }

    // suppose shape is [3][4][5][6] for x,y,z,t. The index is
    // t + z(6) + y(5 * 6) + x(4 * 5 *6).
    // Assume that indices[0] holds x, indices[1] holds y, ... 

    // It's hard to work with Pixes
    vector<int> shape = get_shape_vector(indices.size());

    // We want to work from the rightmost index to the left
    reverse(indices.begin(), indices.end());
    reverse(shape.begin(), shape.end());

    vector<int>::iterator indices_iter = indices.begin();
    vector<int>::iterator shape_iter = shape.begin();

    int index = *indices_iter++; // in the ex. above, this adds `t'
    int multiplier = 1;
    while (indices_iter != indices.end()) {
	multiplier *= *shape_iter++;
	index += multiplier * *indices_iter++;
    }
    
    return index;
}

// get_shape_vector and get_nth_dim_size are public because that are called
// from Grid. 9/14/2001 jhrg

vector<int>
AsciiArray::get_shape_vector(size_t n) throw(InternalErr)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
	bt = this ;

    if (n < 1 || n > bt->dimensions(true)) {
	string msg = "Attempt to get ";
	msg += long_to_string(n) + " dimensions from " + name() 
	    + " which has only " + long_to_string(bt->dimensions(true))
	    + "dimensions.";
    
	throw InternalErr(__FILE__, __LINE__, msg); 
    }

    vector<int> shape(n);
    vector<int>::iterator shape_iter = shape.begin();
    Array::Dim_iter p = bt->dim_begin();
    for (unsigned i = 0; i < n; i++) {
	*shape_iter++ = bt->dimension_size(p++, true);
    }

    return shape;
}

/** Get the size of the Nth dimension. The first dimension is N == 0.
    @param n The index. Uses sero-based indexing.
    @return the size of the dimension. */
int
AsciiArray::get_nth_dim_size(size_t n) throw(InternalErr)
{
    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
	bt = this ;
    // I think this should test 0 ... dimensions(true)-1. jhrg 7/20/
#if 0
    if (n < 1 || n > dimensions(true)) {
#endif
    if (/*n < 0 ||*/ n > bt->dimensions(true)-1) {
	string msg = "Attempt to get dimension ";
	msg += long_to_string(n+1) + " from `" + bt->name() + "' which has " 
	    + long_to_string(bt->dimensions(true)) + " dimension(s).";
	throw InternalErr(__FILE__, __LINE__, msg);
    }

    return bt->dimension_size(bt->dim_begin() + n, true);
}

void 
AsciiArray::print_array(FILE *os, bool /*print_name*/)
{
    DBG(cerr << "Entering AsciiArray::print_array" << endl);

    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
	bt = this ;

    int dims = bt->dimensions(true);
    if (dims <= 1)
	throw InternalErr(__FILE__, __LINE__, 
	  "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of all but the last dimension of
    // the array (not the size; each value is one less that the size).
    vector<int> shape = get_shape_vector(dims - 1);
    int rightmost_dim_size = get_nth_dim_size(dims - 1);

    // state holds the indexes of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row.
    vector<int> state(dims - 1, 0);

    bool more_indices;
    int index = 0;
    do {
	// Print indices for all dimensions except the last one.
	fprintf(os, "%s",
                names->lookup(dynamic_cast<AsciiOutput*>(this)->get_full_name(), translate).c_str());
	for (int i = 0; i < dims - 1; ++i) {
	    fprintf(os, "[%d]", state[i]);
	}
	fprintf(os, ", ");

	index = print_row(os, index, rightmost_dim_size - 1);
	more_indices = increment_state(&state, shape);
	if (more_indices)
	    fprintf(os, "\n");

    } while (more_indices);

    DBG(cerr << "ExitingAsciiArray::print_array" << endl);
}

void 
AsciiArray::print_complex_array(FILE *os, bool /*print_name*/)
{
    DBG(cerr << "Entering AsciiArray::print_complex_array" << endl);

    Array *bt = dynamic_cast<Array *>( _redirect ) ;
    if( !bt )
	bt = this ;

    int dims = bt->dimensions(true);
    if (dims < 1)
	throw InternalErr(__FILE__, __LINE__, 
	  "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of all but the last dimension of
    // the array (not the size; each value is one less that the size).
    vector<int> shape = get_shape_vector(dims);

    vector<int> state(dims, 0);

    bool more_indices;
    do {
	// Print indices for all dimensions except the last one.
	fprintf(os, "%s", 
                names->lookup(dynamic_cast<AsciiOutput*>(this)->get_full_name(), translate).c_str());
	for (int i = 0; i < dims; ++i) {
	    fprintf(os, "[%d]", state[i]);
	}
        fprintf(os, "\n");

	BaseType *curr_var = basetype_to_asciitype( bt->var( get_index( state ) ) );
	dynamic_cast<AsciiOutput*>(curr_var)->print_ascii(os, true);
	
	more_indices = increment_state(&state, shape);
	if (more_indices)
            fprintf(os, "\n");

    } while (more_indices);

    DBG(cerr << "ExitingAsciiArray::print_complex_array" << endl);
}
