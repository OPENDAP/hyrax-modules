
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

#include "config.h"

#include <iostream>
#include <string>

#include "InternalErr.h"
#include "AsciiSequence.h"
#include "AsciiStructure.h"
//#include "name_map.h"
#include "get_ascii.h"
#include "debug.h"

using std::endl ;
using namespace dap_asciival;

BaseType *
AsciiSequence::ptr_duplicate()
{
    return new AsciiSequence(*this);
}

AsciiSequence::AsciiSequence(const string &n) : Sequence(n)
{
}

AsciiSequence::AsciiSequence( Sequence *bt ) : AsciiOutput( bt )
{
    // Let's make the alternative structure of Ascii types now so that we
    // don't have to do it on the fly.
    Vars_iter p = bt->var_begin();
    while( p != bt->var_end() )
    {
	BaseType *new_bt = basetype_to_asciitype( *p ) ;
	add_var( new_bt ) ;
	delete new_bt ;
	p++ ;
    }
    set_name( bt->name() ) ;
}

AsciiSequence::~AsciiSequence()
{
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
    Sequence *seq = dynamic_cast<Sequence *>( _redirect ) ;
    if( !seq )
	seq = this ;

    // Print the values from this sequence.
    int elements = seq->element_count() - 1;
    int j = 0;
    bool done = false;
    do {
	BaseType *bt_ptr = seq->var_value(row, j++);
	if (bt_ptr) {		// Check for data.
	    BaseType *abt_ptr = basetype_to_asciitype( bt_ptr ) ;
	    if (bt_ptr->type() == dods_sequence_c) {
		dynamic_cast<AsciiSequence*>(abt_ptr)->print_ascii_rows
		    (os, outer_vars);
	    }
	    else {
		// push the real base type pointer instead of the ascii one.
		// We can cast it again later from the outer_vars vector.
		outer_vars.push_back(bt_ptr);
		dynamic_cast<AsciiOutput*>(abt_ptr)->print_ascii(os, false);
	    }
	    // we only need the ascii type here, so delete it
	    delete abt_ptr ;
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
    for(BTR_iter = outer_vars.begin(); BTR_iter != outer_vars.end(); BTR_iter++)
    {
	BaseType *abt_ptr = basetype_to_asciitype( *BTR_iter ) ;
	dynamic_cast<AsciiOutput*>(abt_ptr)->print_ascii(os, false);
	fprintf(os, ", ");
	// we only need the ascii base type locally, so delete it
	delete abt_ptr ;
    }
}

void
AsciiSequence::print_ascii_rows(FILE *os, BaseTypeRow outer_vars)
{
    Sequence *seq = dynamic_cast<Sequence *>( _redirect ) ;
    if( !seq )
	seq = this ;

    int rows = seq->number_of_rows() - 1;
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
            fprintf( os, "%s", dynamic_cast<AsciiOutput*>(*p)->get_full_name().c_str() ) ;
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
    Sequence *seq = dynamic_cast<Sequence *>( _redirect ) ;
    if( !seq )
	seq = this ;

    if (seq->is_linear()) {
	if (print_name) {
	    print_header(os);
	    fprintf(os, "\n");
	}
	
	BaseTypeRow outer_vars(0);
	print_ascii_rows(os, outer_vars);
    }
    else {
	int rows = seq->number_of_rows() - 1;
	int elements = seq->element_count() - 1;

	// For each row of the Sequence...
	bool rows_done = false;
	int i = 0;
	do { 
	    // For each variable of the row...
	    bool vars_done = false;
	    int j = 0;
	    do {
		BaseType *bt_ptr = seq->var_value(i, j++);
		BaseType *abt_ptr = basetype_to_asciitype( bt_ptr ) ;
		dynamic_cast<AsciiOutput*>(abt_ptr)->print_ascii(os, true);
		// abt_ptr is not stored for future use, so delete it
		delete abt_ptr ;

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


