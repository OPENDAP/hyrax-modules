
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

// Interface for the class AsciiStructure. See AsciiByte.h
//
// 3/12/98 jhrg

#ifndef _asciistructure_h
#define _asciistructure_h 1

#ifdef _GNUG_
//#pragma interface
#endif

#include "Structure.h"
#include "AsciiOutput.h"

class AsciiStructure: public Structure, public AsciiOutput {
public:
    AsciiStructure(const string &n = (char *)0);
    AsciiStructure( Structure *bt ) ;
    virtual ~AsciiStructure();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
    virtual void print_header(FILE *os);
    virtual void print_ascii(FILE *os, bool print_name = true) 
	throw(InternalErr);
};

// $Log: AsciiStructure.h,v $
// Revision 1.6  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.4.4.2  2002/02/18 19:26:36  jimg
// Fixed bug 329. In cases where a Structure was inside a Sequence,
// print_header was being called (correctly). However, the method flagged
// this as an error because it thought that such a sequence was not `linear.'
// That is, it thought it could not be flattened and printed as a column. I
// fixed AsciiSequence::print_header so that it calls AsciiStructure::print
// _header. Note that the complimentary change is not needed since Structures
// are printed one elment after the other.
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
// Revision 1.3  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.2  1998/09/16 23:31:29  jimg
// Added print_all_vals().
//
// Revision 1.1  1998/03/13 21:25:25  jimg
// Added
//

#endif
