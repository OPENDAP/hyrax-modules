
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWStr. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWStr.cc,v 1.4 2000/10/03 20:07:21 jimg Exp $"};

#include <assert.h>
#include <string>
#include <math.h>
#include <stdlib.h>

#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <string>

#include "InternalErr.h"

#include "WWWStr.h"
#include "WWWOutput.h"

Str *
NewStr(const string &n)
{
    return new WWWStr(n);
}

WWWStr::WWWStr(const string &n) : Str(n)
{
}

BaseType *
WWWStr::ptr_duplicate()
{
    return new WWWStr(*this);
}

bool
WWWStr::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWStr::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}

// $Log: WWWStr.cc,v $
// Revision 1.4  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//
