
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the WWWGrid ctor class. See WWWByte.h
//
// 4/7/99 jhrg

// $Log: WWWGrid.h,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//

#ifndef _WWWGrid_h
#define _WWWGrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"

class WWWGrid: public Grid {
public:
    WWWGrid(const string &n = (char *)0);
    virtual ~WWWGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

#endif


