
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
    WWWGrid(const String &n = (char *)0);
    virtual ~WWWGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

#endif


