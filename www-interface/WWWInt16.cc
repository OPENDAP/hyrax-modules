
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt16. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWInt16.cc,v $
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <String.h>

#include "WWWInt16.h"
#include "WWWOutput.h"

Int16 *
NewInt16(const String &n)
{
    return new WWWInt16(n);
}

WWWInt16::WWWInt16(const String &n) : Int16(n)
{
}

BaseType *
WWWInt16::ptr_duplicate()
{
    return new WWWInt16(*this);
}

bool
WWWInt16::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWInt16::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
