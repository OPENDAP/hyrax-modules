
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// WWWUInt16 interface. See WWWByte.h for more info.
//
// 4/7/99 jhrg

#ifndef _wwwuint16_h
#define _wwwuint16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"
#include "WWWOutput.h"

class WWWUInt16: public UInt16 {
public:
    WWWUInt16(const string &n = (char *)0);
    virtual ~WWWUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);
    
    void print_val(ostream &os, string space = "", bool print_decl_p = true);
};

// $Log: WWWUInt16.h,v $
// Revision 1.4  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#endif

