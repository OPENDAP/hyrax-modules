
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiUInt16 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

// $Log: AsciiUInt16.h,v $
// Revision 1.1  1999/03/29 21:22:55  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:25  jimg
// Added
//

#ifndef _AsciiUInt16_h
#define _AsciiUInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"

class AsciiUInt16: public UInt16 {
public:
    AsciiUInt16(const String &n = (char *)0);
    virtual ~AsciiUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
    
    void print_val(ostream &os, String space = "", bool print_decl_p = true);
};

#endif

