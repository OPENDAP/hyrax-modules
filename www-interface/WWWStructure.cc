
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

static char rcsid[] not_used = {"$Id: WWWStructure.cc,v 1.4 2000/10/03 20:07:21 jimg Exp $"};

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "DAS.h"
#include "InternalErr.h"

#include "WWWStructure.h"
#include "WWWSequence.h"
#include "WWWOutput.h"

extern DAS global_das;
extern WWWOutput wo;

Structure *
NewStructure(const string &n)
{
    return new WWWStructure(n);
}

BaseType *
WWWStructure::ptr_duplicate()
{
    return new WWWStructure(*this);
}

WWWStructure::WWWStructure(const string &n) : Structure(n)
{
}

WWWStructure::~WWWStructure()
{
}

// For this `WWW' class, run the read mfunc for each of variables which
// comprise the structure. 

bool
WWWStructure::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

// As is the case with geturl, use print_all_vals to print all the values of
// a sequence. 

void 
WWWStructure::print_val(ostream &os, string space, bool print_decls)
{
    os << "<b>Structure " << name() << "</b><br>\n";
    os << "<dl><dd>\n";

    for (Pix p = first_var(); p; next_var(p)) {
	var(p)->print_val(os, "", print_decls);
	wo.write_variable_attributes(var(p), global_das);
	os << "<p><p>\n";
    }
    os << "</dd></dl>\n";
}

// Is this a simple WWWStructure? Simple WWWStructures are composed of
// only simple type elements *or* other structures which are simple.

bool
WWWStructure::is_simple_structure()
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->type() == dods_structure_c) {
	    if (!dynamic_cast<WWWStructure *>(var(p))->is_simple_structure())
		return false;
	}
	else {
	    if (!var(p)->is_simple_type())
		return false;
	}
    }

    return true;
}

// $Log: WWWStructure.cc,v $
// Revision 1.4  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//
