
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiFloat64. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiFloat64.cc,v $
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:15  jimg
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

#include "AsciiFloat64.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Float64 *
NewFloat64(const string &n)
{
    return new AsciiFloat64(n);
}

AsciiFloat64::AsciiFloat64(const string &n) : Float64(n)
{
}

BaseType *
AsciiFloat64::ptr_duplicate()
{
    return new AsciiFloat64(*this);
}
 
bool
AsciiFloat64::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
AsciiFloat64::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Float64::print_val(os, "", false);
}
