
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an ASCII
// representation of the data read from a DAP server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// AsciiInt32 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

#ifndef _asciiint32_h
#define _asciiint32_h 1

#include "Int32.h"
#include "AsciiOutput.h"

class AsciiInt32: public Int32, public AsciiOutput {
public:
    AsciiInt32(const string &n = (char *)0) : Int32( n ) {}
    AsciiInt32( Int32 *bt ) : AsciiOutput( bt ) { set_name( bt->name() ) ; }
    virtual ~AsciiInt32() {}

    virtual BaseType *ptr_duplicate();
};

#endif

