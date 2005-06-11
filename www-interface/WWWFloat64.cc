
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of www_int, software which returns an HTML form which
// can be used to build a URL to access data from a DAP data server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// asciival is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
// 
// asciival is distributed in the hope that it will be useful, but WITHOUT
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

// Implementation for WWWFloat64. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWFloat64.cc,v 1.6 2003/12/08 18:08:02 edavis Exp $"};

#include <iostream>
#include <string>

#include <Pix.h>

#include "InternalErr.h"

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
WWWFloat64::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
WWWFloat64::print_val(ostream &os, string, bool print_decl_p)
{
    write_simple_variable(os, (string)name().c_str(), fancy_typename(this));
}

// $Log: WWWFloat64.cc,v $
// Revision 1.6  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.5.4.1  2003/07/11 05:27:02  jimg
// Changed <iostream.h> to <iostream>. This will build with both 3.2, 2.95
// and most other current compilers.
//
// Revision 1.5  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.4.2.1  2002/09/05 22:27:29  pwest
// Removed includes to SLList and DLList. These are not necessary and no longer
// supported.
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
