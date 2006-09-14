
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
#include "name_map.h"
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
#if 0
/** Serialize a Sequence. 

    Leaf Sequences must be marked as such (see DDS::tag_nested_sequence()),
    as must the top most Sequence.

    How the code works. Methods called for various functions are named in
    brackets:
    <ol>
    <li>Sending a one-level sequence:
    <pre>
    Dataset {
        Sequence {
            Int x;
            Int y;
        } flat;
    } case_1;
    </pre>
    
    Serialize it by reading successive rows and sending all of those that
    satisfy the CE. Before each row, send a start of instance (SOI) marker.
    Once all rows have been sent, send an End of Sequence (EOS)
    marker.[serialize_leaf].</li>

    <li>Sending a nested sequence:
    <pre>
    Dataset {
        Sequence {
            Int t;
            Sequence {
                Int z;
            } inner;
        } outer;
    } case_2;
    </pre>
    
    Serialize by reading the first row of outer and storing the values. Do
    not evaluate the CE [serialize_parent_part_one]. Call serialize() for inner
    and read each row for it, evaluating the CE for each row that is read.
    After the first row of inner is read and satisfies the CE, write out the
    SOI marker and values for outer [serialize_parent_part_two], then write 
    the SOI and values for the first row of inner. Continue to read and send
    rows of inner until the last row has been read. Send EOS for inner 
    [serialize_leaf]. Now read the next row of outer and repeat. Once outer\
    is completely read, send its EOS marker.</li>
    </ol>

    Notes:
    <ol>
    <li>For a nested Sequence, the child sequence must follow all other types
    in the parent sequence (like the example). There may be only one nested
    Sequence per level.</li>

    <li>CE evaluation happens only in a leaf sequence.</li>
    
    <li>When no data statisfies a CE, the empty Sequence is signalled by a
    single EOS marker, regardless of the level of nesting of Sequences. That
    is, the EOS marker is sent for only the outer Sequence in the case of a
    completely empty response.</li>
    </ol>
*/
bool
AsciiSequence::transfer_data(const string &dataset, ConstraintEvaluator &eval, 
                             DDS &dds, bool ce_eval)
{
    DBG(cerr << "Entering Sequence::serialize for " << name() << endl);
    
    if (is_leaf_sequence())
        return transfer_data_for_leaf(dataset, dds, eval, ce_eval);
    else 
        return serialize_parent_part_one(dataset, dds, eval, NULL);
}

// We know this is not a leaf Sequence. That means that this Sequence holds
// another Sequence as one of its fields _and_ that child Sequence triggers
// the actual transmission of values.

bool
AsciiSequence::serialize_parent_part_one(const string &dataset, DDS &dds, 
                                    ConstraintEvaluator &eval, XDR *sink)
{
    throw InternalErr(__FILE__, __LINE__, "AsciiSequence::serialize_parent_part_one");

    DBG(cerr << "Entering serialize_parent_part_one for " << name() << endl);
    
    int i = (get_starting_row_number() != -1) ? get_starting_row_number() : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    // Since this is a parent sequence, read the row ignoring the CE (all of
    // the CE clauses will be evaluated by the leaf sequence).
    bool status = read_row(i, dataset, dds, eval, false);
    DBG(cerr << "Sequence::serialize_parent_part_one::read_row() status: " << status << endl);
    
    while ( status && (get_ending_row_number() == -1 || i <= get_ending_row_number()) ) {
        i += get_row_stride();

        // DBG(cerr << "Writing Start of Instance marker" << endl);
        // write_start_of_instance(sink);

        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++)
        {
            // Only call serialize for child Sequences; the leaf sequence
            // will trigger the transmission of values for its parents (this
            // sequence and maybe others) once it gets soem valid data to
            // send. 
            // Note that if the leaf sequence has no variables in the current
            // projection, its serialize() method will never be called and that's
            // the method that triggers actually sending values. Thus the leaf
            // sequence must be the lowest level sequence with values whose send_p
            // property is true.
            if ((*iter)->send_p() && (*iter)->type() == dods_sequence_c)
                (*iter)->serialize(dataset, eval, dds, sink);
        }

        set_read_p(false);      // ...so this will read the next instance

        status = read_row(i, dataset, dds, eval, false);
        DBG(cerr << "Sequence::serialize_parent_part_one::read_row() status: " << status << endl);
    }
    // Reset current row number for next nested sequence element.
    reset_row_number();

#if 0
    // Always write the EOS marker? 12/23/04 jhrg
    // Yes. According to DAP2, a completely empty response is signalled by
    // a return value of only the EOS marker for the outermost sequence.
    if (d_top_most || d_wrote_soi) {
        DBG(cerr << "Writing End of Sequence marker" << endl);
        write_end_of_sequence(sink);
        d_wrote_soi = false;
    }
#endif

    return true;                // Signal errors with exceptions.
}

// If we are here then we know that this is 'parent sequence' and that the
// leaf seq has found valid data to send. We also know that
// serialize_parent_part_one has been called so data are in the instance's
// fields. This is wheree we send data. Whereas ..._part_one() contains a
// loop to iterate over all of rows in a parent sequence, this does not. This
// method assumes that the serialize_leaf() will call it each time it needs
// to be called.
//
// NB: This code only works if the child sequences appear after all other
// variables. 
void
AsciiSequence::serialize_parent_part_two(const string &dataset, DDS &dds, 
                                    ConstraintEvaluator &eval, XDR *sink)
{
    throw InternalErr(__FILE__, __LINE__, "AsciiSequence::serialize_parent_part_two");
    
    DBG(cerr << "Entering serialize_parent_part_two for " << name() << endl);
    
    BaseType *btp = get_parent();
    if (btp && btp->type() == dods_sequence_c)
        dynamic_cast<AsciiSequence&>(*btp).serialize_parent_part_two(dataset, dds, 
                                                                eval, sink);

    if (get_unsent_data()) {
        DBG(cerr << "Writing Start of Instance marker" << endl);
#if 0
        d_wrote_soi = true;
        write_start_of_instance(sink);
#endif        
        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++) {
            // Send all the non-sequence variables
            DBG(cerr << "Sequence::serialize_parent_part_two(), serializing "
                     << (*iter)->name() << endl);
            if ((*iter)->send_p() && (*iter)->type() != dods_sequence_c) {
                DBG(cerr << "Send P is true, sending " << (*iter)->name() << endl);
                (*iter)->serialize(dataset, eval, dds, sink, false);
            }
        }

        set_unsent_data(false);
    }
}

// This code is only run by a leaf sequence. Note that a one level sequence
// is also a leaf sequence.
bool
AsciiSequence::transfer_data_for_leaf(const string &dataset, DDS &dds, 
                                      ConstraintEvaluator &eval, bool ce_eval)
{
    DBG(cerr << "Entering Sequence::serialize_leaf for " << name() << endl);
    int i = (get_starting_row_number() != -1) ? get_starting_row_number() : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    bool status = read_row(i, dataset, dds, eval, ce_eval);
    DBG(cerr << "Sequence::serialize_leaf::read_row() status: " << status << endl);
    
    // Once the first valid (satisfies the CE) row of the leaf sequence has
    // been read, we know we're going to send data. Send the current instance
    // of the parent/ancestor sequences now, if there are any. We only need
    // to do this once, hence it's not inside the while loop, but we only
    // send the parent seq data _if_ there's data in the leaf to send, that's
    // why we wait until after the first call to read_row() here in the leaf
    // sequence.
    //
    // NB: It's important to only call serialize_parent_part_two() for a
    // Sequence that really is the parent of a leaf sequence. The fancy cast
    // will throw and exception if btp is not a Sequence, but doesn't test
    // that it's a parent sequence as we've defined them here.
#if 0
    if ( status && (get_ending_row_number() == -1 || i <= get_ending_row_number()) ) {
        BaseType *btp = get_parent();
        if (btp && btp->type() == dods_sequence_c)
            dynamic_cast<AsciiSequence&>(*btp).serialize_parent_part_two(dataset, 
                                                                    dds, eval,
                                                                    sink);
    }
#endif

    while ( status && (get_ending_row_number() == -1 || i <= get_ending_row_number()) ) {
        i += get_row_stride();

        BaseTypeRow *row_data = new BaseTypeRow;
        
        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++) {
            DBG(cerr << "AsciiSequence::serialize_leaf(), saving "
                     << (*iter)->name() << endl);
            if ((*iter)->send_p()) {
                 DBG(cerr << "Send P is true, sending " << (*iter)->name() << endl);
                 row_data->push_back((*iter)->ptr_duplicate());
            }
        }

        // Save the row_data to the values().
        set_values(row_data);
        
        set_read_p(false);      // ...so this will read the next instance

        status = read_row(i, dataset, dds, eval, ce_eval);
        DBG(cerr << "Sequence::serialize_leaf::read_row() status: " << status << endl);
    }

    return true;                // Signal errors with exceptions.
}
#endif

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


