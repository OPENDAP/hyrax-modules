
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an ASCII
// representation of the data read from a DAP server.

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
 
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for AsciiStr. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef __GNUG__
//#pragma implementation
#endif

#include "config_asciival.h"

#include <string>

#include "InternalErr.h"
#include "AsciiStr.h"

Str *
NewStr(const string &n)
{
    return new AsciiStr(n);
}

AsciiStr::AsciiStr(const string &n) : Str(n)
{
}

BaseType *
AsciiStr::ptr_duplicate()
{
    return new AsciiStr(*this);
}

bool
AsciiStr::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

// $Log: AsciiStr.cc,v $
// Revision 1.6  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.5  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.4.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:18  jimg
// Added
//
