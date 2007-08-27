
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

// Interface to the AsciiGrid ctor class. See AsciiByte.h
//
// 3/12/98 jhrg

#ifndef _asciigrid_h
#define _asciigrid_h 1

#include "Grid.h"
#include "AsciiOutput.h"

class AsciiGrid: public Grid, public AsciiOutput {
private:
    void print_vector(FILE *os, bool print_name);
    void print_vector(ostream &strm, bool print_name);
    void print_grid(FILE *os, bool print_name);
    void print_grid(ostream &strm, bool print_name);

public:
    AsciiGrid(const string &n = (char *)0);
    AsciiGrid( Grid *grid ) ;
    virtual ~AsciiGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual void print_ascii(FILE *os, bool print_name = true)
	throw(InternalErr);
    virtual void print_ascii(ostream &strm, bool print_name = true)
	throw(InternalErr);
};

#endif

