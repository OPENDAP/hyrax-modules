// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for WWWStr type. See WWWByte.h
//
// 4/7/99 jhrg

// $Log: WWWStr.h,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//

#ifndef _WWWStr_h
#define _WWWStr_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "dods-limits.h"
#include "Str.h"
#include "WWWOutput.h"

class WWWStr: public Str {
public:
    WWWStr(const string &n = (char *)0);
    virtual ~WWWStr() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif

