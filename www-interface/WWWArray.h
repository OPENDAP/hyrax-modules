
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface definition for WWWArray. See WWWByte.h for more information
//
// 4/7/99 jhrg

// $Log: WWWArray.h,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:50  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//

#ifndef _WWWArray_h
#define _WWWArray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Array.h"

class WWWArray: public Array {

public:
    WWWArray(const string &n = (char *)0, BaseType *v = 0);
    virtual ~WWWArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);

    /// Overload of BaseType mfunc. This prints arrays using commas and CRs.
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif


