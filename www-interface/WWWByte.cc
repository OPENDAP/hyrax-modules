
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWByte. See the comments in WWWByte.h
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWByte.cc,v 1.5 2001/09/28 23:51:32 jimg Exp $"};

// The NewByte `helper function' creates a pointer to an WWWByte and
// returns that pointer. It takes the same arguments as the class's ctor. If
// any of the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instnace of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these fuctions (in util.h) should *not* need
// changing. 

#include <stdio.h>
#include <assert.h>

#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <string>

#include "InternalErr.h"

#include "WWWByte.h"
#include "WWWOutput.h"

Byte *
NewByte(const string &n)
{
    return new WWWByte(n);
}

WWWByte::WWWByte(const string &n) : Byte(n)
{
}

BaseType *
WWWByte::ptr_duplicate()
{
    return new WWWByte(*this);
}

bool
WWWByte::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWByte::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, name(), fancy_typename(this));
}

// $Log: WWWByte.cc,v $
// Revision 1.5  2001/09/28 23:51:32  jimg
// Merged with 3.2.4.
//
// Revision 1.4.2.1  2001/09/10 19:32:28  jimg
// Fixed two problems: 1) Variable names in the JavaScript code sometimes
// contained spaces since they were made using the dataset's variable name.
// The names are now filtered through id2www and esc2underscore. 2) The CE
// sometimes contained spaces, again, because dataset variable names were
// used to build the CE. I filtered the names with id2www_ce before passing
// them to the JavaScript code.
//
// Revision 1.4  2000/10/03 20:07:20  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:02  jimg
// First version
//
