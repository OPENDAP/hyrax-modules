
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface definition for AsciiArray. See AsciiByte.h for more information
//
// 3/12/98 jhrg

#ifndef _asciiarray_h
#define _asciiarray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Array.h"
#include "AsciiOutput.h"

class AsciiArray: public Array, public AsciiOutput {
private:
    void print_vector(ostream &os, bool print_name);
    void print_array(ostream &os, bool print_name);
    void print_complex_array(ostream &os, bool print_name);

public:
    AsciiArray(const string &n = (char *)0, BaseType *v = 0);
    virtual ~AsciiArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);

    int print_row(ostream &os, int index, int number);

    int get_index(vector<int> indices) throw(InternalErr);

    /** Get the size of dimension #n#. 
	@param n Return the size of the n^{th} dimension.
	@return The size of the n^{th} dimension.
	@exception InternalErr. */
    int get_nth_dim_size(size_t n) throw(InternalErr);

    /** Get the sizes of the first N dimensions of this array. This
	`shape vector' may be used in all sorts of output formatters.
	@return A vector describing the shape of the array. Each value
	contains the highest index value. To get the size, add one. */
    vector<int> get_shape_vector(size_t n) throw(InternalErr);

    virtual void AsciiArray::print_ascii(ostream &os, bool print_name = true) 
	throw(InternalErr);
};

// $Log: AsciiArray.h,v $
// Revision 1.4  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.3.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.3  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.2  1999/04/30 17:06:53  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:20  jimg
// Added
//

#endif


