
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt16. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWInt16.cc,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <string>

#include "InternalErr.h"

#include "WWWInt16.h"
#include "WWWOutput.h"

Int16 *
NewInt16(const string &n)
{
    return new WWWInt16(n);
}

WWWInt16::WWWInt16(const string &n) : Int16(n)
{
}

BaseType *
WWWInt16::ptr_duplicate()
{
    return new WWWInt16(*this);
}

bool
WWWInt16::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWInt16::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}
