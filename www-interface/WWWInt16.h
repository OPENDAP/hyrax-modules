
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// WWWInt16 interface. See WWWByte.h for more info.
//
// 4/7/99 jhrg

// $Log: WWWInt16.h,v $
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//

#ifndef _WWWInt16_h
#define _WWWInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int16.h"
#include "WWWOutput.h"

class WWWInt16: public Int16 {
public:
    WWWInt16(const string &n = (char *)0);
    virtual ~WWWInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
    
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif

