
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt16. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiInt16.cc,v $
// Revision 1.1  1999/03/29 21:22:54  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:17  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <String.h>

#include "AsciiInt16.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Int16 *
NewInt16(const String &n)
{
    return new AsciiInt16(n);
}

AsciiInt16::AsciiInt16(const String &n) : Int16(n)
{
}

BaseType *
AsciiInt16::ptr_duplicate()
{
    return new AsciiInt16(*this);
}

bool
AsciiInt16::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiInt16::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Int16::print_val(os, "", false);
}
