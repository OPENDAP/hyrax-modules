
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt16. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiUInt16.cc,v $
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1999/03/29 21:22:54  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

#include <iostream.h>
#include <string>

#include "AsciiUInt16.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

UInt16 *
NewUInt16(const string &n)
{
    return new AsciiUInt16(n);
}

AsciiUInt16::AsciiUInt16(const string &n) : UInt16(n)
{
}

BaseType *
AsciiUInt16::ptr_duplicate()
{
    return new AsciiUInt16(*this);
}

bool
AsciiUInt16::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
AsciiUInt16::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    UInt16::print_val(os, "", false);
}
