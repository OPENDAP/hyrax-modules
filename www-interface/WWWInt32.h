
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// WWWInt32 interface. See WWWByte.h for more info.
//
// 4/7/99 jhrg

// $Log: WWWInt32.h,v $
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//

#ifndef _WWWInt32_h
#define _WWWInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"
#include "WWWOutput.h"

class WWWInt32: public Int32 {
public:
    WWWInt32(const string &n = (char *)0);
    virtual ~WWWInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
    
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif

