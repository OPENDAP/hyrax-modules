
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiArray. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiArray.cc,v $
// Revision 1.1  1998/03/13 21:25:14  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiArray.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Array *
NewArray(const String &n, BaseType *v)
{
    return new AsciiArray(n, v);
}

BaseType *
AsciiArray::ptr_duplicate()
{
    return new AsciiArray(*this);
}

AsciiArray::AsciiArray(const String &n, BaseType *v) : Array(n, v)
{
}

AsciiArray::~AsciiArray()
{
}

bool
AsciiArray::read(const String &, int &)
{
    assert(false);
    return false;
}

int
AsciiArray::print_vector(ostream &os, int index, int number)
{
    for (unsigned i = 0; i < number-1; ++i) {
	var(index++)->print_val(os, "", false);
	os << ", ";
    }
    var(index++)->print_val(os, "", false);

    return index;
}

void 
AsciiArray::print_val(ostream &os, String, bool print_decl_p)
{
    // Print the name and size of the variable. Note that I print the size
    // information while initializing the vector #shape#. 3/9/98 jhrg
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    
    // Set up the shape and state vectors. Shape holds the shape of this
    // array, state holds the index of the current vector to print.
    int dims = dimensions(true);
    int *shape = new int[dims];
    int state_dims = dims - 1;
    int *state = new int[state_dims];

    Pix p = first_dim();
    for (int i = 0; p; ++i, next_dim(p)) {
	shape[i] = dimension_size(p, true);
	if (print_decl_p)
	    os << "[" << shape[i] << "]"; // Print the variable's shape
    }
    
    os << endl;			// End name/shape output.

    // Initialize the state vector. Used to keep track of printing.
    for (int i = 0; i < state_dims; ++i)
	state[i] = 0;

    int j;
    int index = 0;
    do {
	// Print indices for all dimensions except the last one. For vectors
	// simply print a list of values.
	if (state_dims >= 1) {
	    for (int i = 0; i < state_dims; ++i) {
		os << "[" << state[i] << "]";
	    }
	    os << ", ";
	}

	// Print the row vector.
	index = print_vector(os, index, shape[dims-1]);
	os << endl;

	// Increment state.
	for (j = state_dims-1; j >= 0; --j) {
	    if (state[j] == shape[j]-1)
		state[j] = 0;
	    else {
		state[j]++;
		break;
	    }
	}

    } while (!(j == -1 && (state_dims == 0 || state[0] == 0)));
}
