
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWFloat32. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWFloat32.cc,v $
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <String.h>

#include "WWWFloat32.h"
#include "WWWOutput.h"

Float32 *
NewFloat32(const String &n)
{
    return new WWWFloat32(n);
}

WWWFloat32::WWWFloat32(const String &n) : Float32(n)
{
}

BaseType *
WWWFloat32::ptr_duplicate()
{
    return new WWWFloat32(*this);
}
 
bool
WWWFloat32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWFloat32::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
