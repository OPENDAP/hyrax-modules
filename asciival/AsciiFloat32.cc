
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiFloat32. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiFloat32.cc,v $
// Revision 1.1  1999/03/29 21:22:55  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:15  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <String.h>

#include "AsciiFloat32.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Float32 *
NewFloat32(const String &n)
{
    return new AsciiFloat32(n);
}

AsciiFloat32::AsciiFloat32(const String &n) : Float32(n)
{
}

BaseType *
AsciiFloat32::ptr_duplicate()
{
    return new AsciiFloat32(*this);
}
 
bool
AsciiFloat32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiFloat32::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Float32::print_val(os, "", false);
}
