
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt32. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWUInt32.cc,v $
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "WWWUInt32.h"
#include "WWWOutput.h"

UInt32 *
NewUInt32(const String &n)
{
    return new WWWUInt32(n);
}

WWWUInt32::WWWUInt32(const String &n) : UInt32(n)
{
}

BaseType *
WWWUInt32::ptr_duplicate()
{
    return new WWWUInt32(*this);
}

bool
WWWUInt32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWUInt32::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
