
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt16. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiUInt16.cc,v $
// Revision 1.1  1999/03/29 21:22:54  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>

#include <String.h>

#include "AsciiUInt16.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

UInt16 *
NewUInt16(const String &n)
{
    return new AsciiUInt16(n);
}

AsciiUInt16::AsciiUInt16(const String &n) : UInt16(n)
{
}

BaseType *
AsciiUInt16::ptr_duplicate()
{
    return new AsciiUInt16(*this);
}

bool
AsciiUInt16::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiUInt16::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    UInt16::print_val(os, "", false);
}
