
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the AsciiGrid ctor class. See AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiGrid.h,v $
// Revision 1.1  1998/03/13 21:25:22  jimg
// Added
//

#ifndef _AsciiGrid_h
#define _AsciiGrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"

class AsciiGrid: public Grid {
public:
    AsciiGrid(const String &n = (char *)0);
    virtual ~AsciiGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

#endif


