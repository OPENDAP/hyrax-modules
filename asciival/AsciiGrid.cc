
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for AsciiGrid. See AsciiByte.
//
// 3/12/98 jhrg

// $Log: AsciiGrid.cc,v $
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:16  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <string>

#include "AsciiGrid.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Grid *
NewGrid(const string &n)
{
    return new AsciiGrid(n);
}

BaseType *
AsciiGrid::ptr_duplicate()
{
    return new AsciiGrid(*this);
}

AsciiGrid::AsciiGrid(const string &n) : Grid(n)
{
}

AsciiGrid::~AsciiGrid()
{
}

bool
AsciiGrid::read(const string &, int &)
{
    assert(false);
    return false;
}

void
AsciiGrid::print_val(ostream &os, string space, bool print_decl_p)
{
    array_var()->print_val(os, space, print_decl_p);

    for (Pix p = first_map_var(); p; next_map_var(p))
	map_var(p)->print_val(os, space, print_decl_p);
}
