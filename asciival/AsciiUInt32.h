
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiUInt32 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

// $Log: AsciiUInt32.h,v $
// Revision 1.1  1998/03/13 21:25:25  jimg
// Added
//

#ifndef _AsciiUInt32_h
#define _AsciiUInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt32.h"

class AsciiUInt32: public UInt32 {
public:
    AsciiUInt32(const String &n = (char *)0);
    virtual ~AsciiUInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
    
    void print_val(ostream &os, String space = "", bool print_decl_p = true);
};

#endif

