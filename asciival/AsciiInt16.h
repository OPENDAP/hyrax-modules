
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiInt16 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

// $Log: AsciiInt16.h,v $
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1999/03/29 21:22:54  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:23  jimg
// Added
//

#ifndef _AsciiInt16_h
#define _AsciiInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int16.h"

class AsciiInt16: public Int16 {
public:
    AsciiInt16(const string &n = (char *)0);
    virtual ~AsciiInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
    
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif

