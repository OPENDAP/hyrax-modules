
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the class WWWSequence. See WWWByte.h
//
// 4/7/99 jhrg

// $Log: WWWSequence.h,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//

#ifndef _WWWSequence_h
#define _WWWSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"

class WWWSequence: public Sequence {
private:
    bool is_simple_sequence();
    void print_header(ostream &os);

public:
    WWWSequence(const string &n = (char *)0);
    virtual ~WWWSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);

    virtual int length();

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif
