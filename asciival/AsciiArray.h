
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface definition for AsciiArray. See AsciiByte.h for more information
//
// 3/12/98 jhrg

// $Log: AsciiArray.h,v $
// Revision 1.2  1999/04/30 17:06:53  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:20  jimg
// Added
//

#ifndef _AsciiArray_h
#define _AsciiArray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Array.h"

class AsciiArray: public Array {
private:
    /// Helper function used by this class' overload of print_val().
    int print_vector(ostream &os, int index, int number);

public:
    AsciiArray(const string &n = (char *)0, BaseType *v = 0);
    virtual ~AsciiArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset, int &error);

    /// Overload of BaseType mfunc. This prints arrays using commas and CRs.
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif


