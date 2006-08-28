
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

// Implementation for the class AsciiStructure. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef _GNUG_
//#pragma implementation
#endif

#include "config_asciival.h"

#include <iostream>
#include <string>

using std::endl ;

#include "InternalErr.h"
#include "AsciiSequence.h"
#include "AsciiStructure.h"
#include "name_map.h"

extern bool translate;
extern name_map *names;

#if 0
Sequence *
NewSequence(const string &n)
{
    return new AsciiSequence(n);
}
#endif

BaseType *
AsciiSequence::ptr_duplicate()
{
    return new AsciiSequence(*this);
}

AsciiSequence::AsciiSequence(const string &n) : Sequence(n)
{
}

AsciiSequence::~AsciiSequence()
{
}

bool 
AsciiSequence::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

int
AsciiSequence::length()
{
    return -1;
}

// case 1: Simple, Seq - handled
// Case 2: Seq, Simple
void
AsciiSequence::print_ascii_row(FILE *os, int row, BaseTypeRow outer_vars)
{
    // Print the values from this sequence.
    int elements = element_count() - 1;
    int j = 0;
    bool done = false;
    do {
	BaseType *bt_ptr = var_value(row, j++);
	if (bt_ptr) {		// Check for data.
	    if (bt_ptr->type() == dods_sequence_c) {
		dynamic_cast<AsciiSequence*>(bt_ptr)->print_ascii_rows
		    (os, outer_vars);
	    }
	    else {
		outer_vars.push_back(bt_ptr);
		dynamic_cast<AsciiOutput*>(bt_ptr)->print_ascii(os, false);
	    }
	}

	// When we're finally done, set the flag and omit the comma.
	if (j > elements)
	    done = true;
	else {
	    fprintf(os, ", ");
	}
    } while (!done);
}

void
AsciiSequence::print_leading_vars(FILE *os, BaseTypeRow &outer_vars)
{
    BaseTypeRow::iterator BTR_iter = outer_vars.begin();
    for (BTR_iter = outer_vars.begin(); BTR_iter != outer_vars.end(); 
	 BTR_iter++) {
	dynamic_cast<AsciiOutput*>(*BTR_iter)->print_ascii(os, false);
	fprintf(os, ", ");
    }
}

void
AsciiSequence::print_ascii_rows(FILE *os, BaseTypeRow outer_vars)
{
    int rows = number_of_rows() - 1;
    int i = 0;
    bool done = false;
    do {
	if (i > 0 && !outer_vars.empty())
	    print_leading_vars(os, outer_vars);

	print_ascii_row(os, i++, outer_vars);

	if (i > rows)
	    done = true;
	else
	    fprintf(os, "\n");
    } while (!done);
}

// Assume that this mfunc is called only for simple sequences. Do not print
// table style headers for complex sequences. 

void
AsciiSequence::print_header(FILE *os)
{
    Vars_iter p = var_begin();
    while (p != var_end()) {
	if ((*p)->is_simple_type())
	    fprintf(os, "%s",
                names->lookup(dynamic_cast<AsciiOutput*>((*p))->get_full_name(), translate).c_str());
	else if ((*p)->type() == dods_sequence_c)
	    dynamic_cast<AsciiSequence *>((*p))->print_header(os);
	else if ((*p)->type() == dods_structure_c)
	    dynamic_cast<AsciiStructure *>((*p))->print_header(os);
	else
	    throw InternalErr(__FILE__, __LINE__,
"This method should only be called by instances for which `is_simple_sequence' returns true.");
	if (++p != var_end())
	    fprintf(os, ", ");
    }
}

void
AsciiSequence::print_ascii(FILE *os, bool print_name) throw(InternalErr)
{
    if (is_linear()) {
	if (print_name) {
	    print_header(os);
	    fprintf(os, "\n");
	}
	
	BaseTypeRow outer_vars(0);
	print_ascii_rows(os, outer_vars);
    }
    else {
	int rows = number_of_rows() - 1;
	int elements = element_count() - 1;

	// For each row of the Sequence...
	bool rows_done = false;
	int i = 0;
	do { 
	    // For each variable of the row...
	    bool vars_done = false;
	    int j = 0;
	    do {
		BaseType *bt_ptr = var_value(i, j++);
		dynamic_cast<AsciiOutput*>(bt_ptr)->print_ascii(os, true);

		if (j > elements)
		    vars_done = true;
		else
		    fprintf(os, "\n");
	    } while (!vars_done);
	    
	    i++;
	    if (i > rows)
		rows_done = true;
	    else
		fprintf(os, "\n");
	} while (!rows_done);
    }
}

// $Log: AsciiSequence.cc,v $
// Revision 1.7  2004/02/03 17:23:40  jimg
// Removed Pix code which was not building (???). Removed AsciiList from
// Build.
//
// Revision 1.6  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.4.4.3  2002/12/18 23:41:25  pwest
// gcc3.2 compile corrections, mainly regarding using statements
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
// Revision 1.3  1999/07/28 23:00:53  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:18  jimg
// Added
//

