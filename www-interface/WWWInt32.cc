
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt32. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWInt32.cc,v $
// Revision 1.1  1999/04/20 00:21:03  jimg
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

#include "WWWInt32.h"
#include "WWWOutput.h"

Int32 *
NewInt32(const String &n)
{
    return new WWWInt32(n);
}

WWWInt32::WWWInt32(const String &n) : Int32(n)
{
}

BaseType *
WWWInt32::ptr_duplicate()
{
    return new WWWInt32(*this);
}

bool
WWWInt32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWInt32::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
