// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for WWWFloat32 type. See WWWByte.h
//
// 4/7/99 jhrg

// $Log: WWWFloat32.h,v $
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//

#ifndef _WWWFloat32_h
#define _WWWFloat32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float32.h"
#include "WWWOutput.h"

class WWWFloat32: public Float32 {
public:
    WWWFloat32(const String &n = (char *)0);
    virtual ~WWWFloat32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

#endif

