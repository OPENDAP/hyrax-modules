
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWStr. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWStr.cc,v $
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "WWWStr.h"
#include "WWWOutput.h"

Str *
NewStr(const String &n)
{
    return new WWWStr(n);
}

WWWStr::WWWStr(const String &n) : Str(n)
{
}

BaseType *
WWWStr::ptr_duplicate()
{
    return new WWWStr(*this);
}

bool
WWWStr::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
WWWStr::print_val(ostream &os, String, bool print_decl_p)
{
    write_simple_variable(os, (string)name().chars(), fancy_typename(this));
}
