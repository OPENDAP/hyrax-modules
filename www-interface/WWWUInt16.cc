
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt16. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWUInt16.cc,v $
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>

#include <String.h>

#include "WWWUInt16.h"
#include "WWWOutput.h"

UInt16 *
NewUInt16(const String &n)
{
    return new WWWUInt16(n);
}

WWWUInt16::WWWUInt16(const String &n) : UInt16(n)
{
}

BaseType *
WWWUInt16::ptr_duplicate()
{
    return new WWWUInt16(*this);
}

bool
WWWUInt16::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWUInt16::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
