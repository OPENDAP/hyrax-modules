
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiInt32 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

// $Log: AsciiInt32.h,v $
// Revision 1.1  1998/03/13 21:25:23  jimg
// Added
//

#ifndef _AsciiInt32_h
#define _AsciiInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"

class AsciiInt32: public Int32 {
public:
    AsciiInt32(const String &n = (char *)0);
    virtual ~AsciiInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
    
    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

#endif

