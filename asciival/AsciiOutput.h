
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 2001
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// An interface which can be used to print ascii representations of DAP
// variables. 9/12/2001 jhrg

#ifndef _asciioutput_h
#define _asciioutput_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream>
#include <vector>

#include "InternalErr.h"

using std::vector;
using std::ostream;

class AsciiOutput {
public:
    virtual ~AsciiOutput() {}

    /** Get the fully qualified name of this object. Names of nested
	constructor types are separated by dots (.).

	@return The fully qualified name of this object. */
    string get_full_name();

    /** Print an ASCII representation for an instance of BaseType's children.
	This version prints the suggested output only for simple types.
	Complex types should overload this with their own definition. 

	The caller of this method is responsible for adding a trialing comma
	where appropriate.

	@param os Write to this stream.
	@param print_name If True, write the name of the variable, a comma
	and then the value. If False, simply write the value. */
    virtual void print_ascii(ostream &os, bool print_name = true) 
	throw(InternalErr); 

    /** Increment #state# to the next value given #shape#. This method
	uses simple modulo arithmetic to provide a way to iterate over all
	combinations of dimensions of an Array or Grid. The vector #shape#
	holds the maximum sizes of each of N dimensions. The vector #state#
	holds the current index values of those N dimensions. Calling this
	method increments #state# to t he next dimension, varying the
	rightmost fastest. 

	To print DODS Array and Grid objects according to the DAP 2.0
	specification, #state# and #shape# should be vectors of length N-1
	for an object of dimension N. 
	@return True if there are more states, false if not. */
    bool increment_state(vector<int> *state, const vector<int> &shape);
};

// $Log: AsciiOutput.h,v $
// Revision 1.2  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.1.2.1  2001/09/18 22:06:16  jimg
// Added
//

#endif
