
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for AsciiGrid. See AsciiByte.
//
// 3/12/98 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

#include <iostream>
#include <string>

#include "InternalErr.h"
#include "AsciiGrid.h"
#include "AsciiArray.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Grid *
NewGrid(const string &n)
{
    return new AsciiGrid(n);
}

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
AsciiGrid::print_ascii(ostream &os, bool print_name) throw(InternalErr)
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
AsciiGrid::print_vector(ostream &os, bool print_name)
{
    dynamic_cast<AsciiArray *>(map_var(first_map_var()))
	->print_ascii(os, print_name);
    
    os << endl;

    dynamic_cast<AsciiArray *>(array_var())->print_ascii(os, print_name);
}

void 
AsciiGrid::print_grid(ostream &os, bool print_name)
{
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

    // state holds the indeces of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row.
    vector<int> state(dims - 1, 0);

    // Now that we have the number of dims, get and print the rightmost map.
    // This is cumbersome; if we used the STL it would be much less so.
    {
	// Use the empty loop to get a Pix to the last element of map_var.
	Pix last = first_map_var();
	for (int i = 0; i < dims - 1; ++i, next_map_var(last)); // empty loop
	// By definition, a map is a vector. Print the rightmost map.
	dynamic_cast<AsciiArray *>(map_var(last))
	    ->print_ascii(os, print_name);
	os << endl;
    }

    bool more_indices;
    int index = 0;
    do {
	// Print indices for all dimensions except the last one. Include the
	// name of the corresponding map vector and the *value* of this
	// index. Note that the successive elements of state give the indeces
	// of each of the N-1 dimensions for the current row.
	os << names.lookup(dynamic_cast<AsciiOutput*>(grid_array)->get_full_name(), translate);
	vector<int>::iterator state_iter = state.begin();
	Pix p = first_map_var();
	for (state_iter = state.begin(); state_iter < state.end(); 
	     state_iter++, next_map_var(p)) {
	    os << "[" 
	       << dynamic_cast<AsciiOutput*>(map_var(p))->get_full_name() 
	       << "=";
	    AsciiArray *m = dynamic_cast<AsciiArray*>(map_var(p));
	    dynamic_cast<AsciiOutput*>(m->var(*state_iter))
		->print_ascii(os, false);
	    os << "]";
	}
	os << ", ";

	index = grid_array->print_row(os, index, rightmost_dim_size - 1);
	more_indices = increment_state(&state, shape);
	if (more_indices)
	    os << endl;

    } while (more_indices);
}

// $Log: AsciiGrid.cc,v $
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
