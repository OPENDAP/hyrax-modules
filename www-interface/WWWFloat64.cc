
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWFloat64. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWFloat64.cc,v $
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "WWWFloat64.h"
#include "WWWOutput.h"

Float64 *
NewFloat64(const string &n)
{
    return new WWWFloat64(n);
}

WWWFloat64::WWWFloat64(const string &n) : Float64(n)
{
}

BaseType *
WWWFloat64::ptr_duplicate()
{
    return new WWWFloat64(*this);
}
 
bool
WWWFloat64::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
WWWFloat64::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}
