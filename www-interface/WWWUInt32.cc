
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt32. See WWWByte.cc
//
// 4/7/99 jhrg

// $Log: WWWUInt32.cc,v $
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
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
#include <string>

#include "InternalErr.h"

#include "WWWUInt32.h"
#include "WWWOutput.h"

UInt32 *
NewUInt32(const string &n)
{
    return new WWWUInt32(n);
}

WWWUInt32::WWWUInt32(const string &n) : UInt32(n)
{
}

BaseType *
WWWUInt32::ptr_duplicate()
{
    return new WWWUInt32(*this);
}

bool
WWWUInt32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWUInt32::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}
