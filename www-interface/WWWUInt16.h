
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// WWWUInt16 interface. See WWWByte.h for more info.
//
// 4/7/99 jhrg

// $Log: WWWUInt16.h,v $
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#ifndef _WWWUInt16_h
#define _WWWUInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"
#include "WWWOutput.h"

class WWWUInt16: public UInt16 {
public:
    WWWUInt16(const String &n = (char *)0);
    virtual ~WWWUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
    
    void print_val(ostream &os, String space = "", bool print_decl_p = true);
};

#endif

