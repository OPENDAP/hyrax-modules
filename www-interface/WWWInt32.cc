
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWInt32. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWInt32.cc,v 1.4 2000/10/03 20:07:20 jimg Exp $"};

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "InternalErr.h"

#include "WWWInt32.h"
#include "WWWOutput.h"

Int32 *
NewInt32(const string &n)
{
    return new WWWInt32(n);
}

WWWInt32::WWWInt32(const string &n) : Int32(n)
{
}

BaseType *
WWWInt32::ptr_duplicate()
{
    return new WWWInt32(*this);
}

bool
WWWInt32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWInt32::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}

// $Log: WWWInt32.cc,v $
// Revision 1.4  2000/10/03 20:07:20  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:03  jimg
// First version
//
