
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt32. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiUInt32.cc,v $
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiUInt32.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

UInt32 *
NewUInt32(const String &n)
{
    return new AsciiUInt32(n);
}

AsciiUInt32::AsciiUInt32(const String &n) : UInt32(n)
{
}

BaseType *
AsciiUInt32::ptr_duplicate()
{
    return new AsciiUInt32(*this);
}

bool
AsciiUInt32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiUInt32::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    UInt32::print_val(os, "", false);
}
