// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for AsciiFloat32 type. See AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiFloat32.h,v $
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1999/03/29 21:22:55  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:22  jimg
// Added
//

#ifndef _AsciiFloat32_h
#define _AsciiFloat32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float32.h"

class AsciiFloat32: public Float32 {
public:
    AsciiFloat32(const string &n = (char *)0);
    virtual ~AsciiFloat32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif

