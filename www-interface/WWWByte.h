
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This set of subclasses is used to build a simple program which will
// generate an HTML form used to query a dataset
//
// 4/7/99 jhrg

// $Log: WWWByte.h,v $
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//

#ifndef _WWWByte_h
#define _WWWByte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Byte.h"

class WWWByte: public Byte {
public:
    WWWByte(const String &n = (char *)0);
    virtual ~WWWByte() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

#endif

