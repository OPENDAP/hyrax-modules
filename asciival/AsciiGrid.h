
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the AsciiGrid ctor class. See AsciiByte.h
//
// 3/12/98 jhrg

#ifndef _asciigrid_h
#define _asciigrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"
#include "AsciiOutput.h"

class AsciiGrid: public Grid, public AsciiOutput {
private:
    void print_vector(ostream &os, bool print_name);
    void print_grid(ostream &os, bool print_name);
#if 0
    int print_row(ostream &os, int index, int number);
#endif

public:
    AsciiGrid(const string &n = (char *)0);
    virtual ~AsciiGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
    virtual void print_ascii(ostream &os, bool print_name = true)
	throw(InternalErr);
};

// $Log: AsciiGrid.h,v $
// Revision 1.4  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.3.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.3  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:22  jimg
// Added
//

#endif


