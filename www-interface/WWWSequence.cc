
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class WWWStructure. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWSequence.cc,v 1.4 2000/10/03 20:07:21 jimg Exp $"};

#include <assert.h>

#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "DAS.h"
#include "InternalErr.h"

#include "WWWSequence.h"
#include "WWWOutput.h"

extern DAS global_das;
extern WWWOutput wo;

Sequence *
NewSequence(const string &n)
{
    return new WWWSequence(n);
}

BaseType *
WWWSequence::ptr_duplicate()
{
    return new WWWSequence(*this);
}

WWWSequence::WWWSequence(const string &n) : Sequence(n)
{
}

WWWSequence::~WWWSequence()
{
}

bool 
WWWSequence::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

int
WWWSequence::length()
{
    return -1;
}

bool
WWWSequence::is_simple_sequence()
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->type() == dods_sequence_c) {
	    if (!dynamic_cast<WWWSequence *>(var(p))->is_simple_sequence())
		return false;
	}
	else {
	    if (!var(p)->is_simple_type())
		return false;
	}
    }

    return true;
}

// As is the case with geturl, use print_all_vals to print all the values of
// a sequence. 

void 
WWWSequence::print_val(ostream &os, string space, bool print_decls)
{
    os << "<b>Sequence " << name() << "</b><br>\n";
    os << "<dl><dd>\n";

    for (Pix p = first_var(); p; next_var(p)) {
	var(p)->print_val(os, "", print_decls);
	wo.write_variable_attributes(var(p), global_das);
	os << "<p><p>\n";
    }

    os << "</dd></dl>\n";
}

// $Log: WWWSequence.cc,v $
// Revision 1.4  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//
