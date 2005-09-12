
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

// Interface for WWWFloat32 type. See WWWByte.h
//
// 4/7/99 jhrg

#ifndef _wwwfloat32_h
#define _wwwfloat32_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
//#pragma interface
#endif
#endif

#include "Float32.h"
#include "WWWOutput.h"

class WWWFloat32: public Float32 {
public:
    WWWFloat32(const string &n = (char *)0);
    virtual ~WWWFloat32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
};

// $Log: WWWFloat32.h,v $
// Revision 1.6  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.5.4.1  2003/06/26 07:53:39  rmorris
// #ifdef'd out //#pragma interface directive under OS X.  It makes the dynamic
// typing system not work.
//
// Revision 1.5  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
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

#endif

