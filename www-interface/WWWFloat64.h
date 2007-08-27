
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

// Interface for WWWFloat64 type. See WWWByte.h
//
// 4/7/99 jhrg

#ifndef _wwwfloat64_h
#define _wwwfloat64_h 1

#include "Float64.h"

class WWWFloat64: public Float64 {
public:
    WWWFloat64(const string &n = (char *)0);
    WWWFloat64( Float64 *bt ) : Float64( bt->name() ) {}
    virtual ~WWWFloat64() {}

    virtual BaseType *ptr_duplicate();

    virtual void print_val(FILE *os, string space = "", 
			   bool print_decl_p = true);
    virtual void print_val(ostream &strm, string space = "", 
			   bool print_decl_p = true);
};

#endif

