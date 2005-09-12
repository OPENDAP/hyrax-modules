
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of www_int, software which returns an HTML form which
// can be used to build a URL to access data from a DAP data server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// www_int is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
// 
// www_int is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
// 
// You should have received a copy of the GNU General Public License along
// with GCC; see the file COPYING. If not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for WWWByte. See the comments in WWWByte.h
//
// 4/7/99 jhrg

#ifdef __GNUG__
//#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id$"};

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

#include <iostream>
#include <string>

#include <Pix.h>

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
// Revision 1.7  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.6.4.1  2003/07/11 05:27:02  jimg
// Changed <iostream.h> to <iostream>. This will build with both 3.2, 2.95
// and most other current compilers.
//
// Revision 1.6  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.4.2.2  2002/09/05 22:27:29  pwest
// Removed includes to SLList and DLList. These are not necessary and no longer
// supported.
//
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
