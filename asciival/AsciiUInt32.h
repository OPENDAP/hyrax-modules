
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiUInt32 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

#ifndef _asciiuint32_h
#define _asciiuint32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt32.h"

class AsciiUInt32: public UInt32 {
public:
    AsciiUInt32(const string &n = (char *)0);
    virtual ~AsciiUInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);
    
    void print_val(ostream &os, string space = "", bool print_decl_p = true);
};

// $Log: AsciiUInt32.h,v $
// Revision 1.3  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:25  jimg
// Added
//

#endif

