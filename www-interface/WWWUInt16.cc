
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt16. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWUInt16.cc,v $
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>

#include <string>

#include "WWWUInt16.h"
#include "WWWOutput.h"

UInt16 *
NewUInt16(const string &n)
{
    return new WWWUInt16(n);
}

WWWUInt16::WWWUInt16(const string &n) : UInt16(n)
{
}

BaseType *
WWWUInt16::ptr_duplicate()
{
    return new WWWUInt16(*this);
}

bool
WWWUInt16::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
WWWUInt16::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}
