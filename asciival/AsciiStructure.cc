
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class AsciiStructure. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiStructure.cc,v $
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//

#ifdef _GNUG_
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiStructure.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Structure *
NewStructure(const String &n)
{
    return new AsciiStructure(n);
}

BaseType *
AsciiStructure::ptr_duplicate()
{
    return new AsciiStructure(*this);
}

AsciiStructure::AsciiStructure(const String &n) : Structure(n)
{
}

AsciiStructure::~AsciiStructure()
{
}

// For this `Ascii' class, run the read mfunc for each of variables which
// comprise the structure. 

bool
AsciiStructure::read(const String &, int &)
{
    assert(false);
    return false;
}

// Is this a simple AsciiStructure? Simple AsciiStructures are composed of
// only simple type elements *or* other structures which are simple.

bool
AsciiStructure::is_simple_structure()
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->type() == dods_structure_c) {
	    if (!((AsciiStructure *)var(p))->is_simple_structure())
		return false;
	}
	else {
	    if (!var(p)->is_simple_type())
		return false;
	}
    }

    return true;
}

// Assume that this mfunc is called only for simple sequences. Do not print
// table style headers for complex sequences. 

void
AsciiStructure::print_header(ostream &os)
{
    for (Pix p = first_var(); p; next_var(p), (void)(p && os << ", "))
	if (var(p)->is_simple_type())
	    os << names.lookup(var(p)->name(), translate);
	else if (var(p)->type() == dods_structure_c)
	    ((AsciiStructure *)var(p))->print_header(os);
}

// As is the case with geturl, use print_all_vals to print all the values of
// a sequence. 

void 
AsciiStructure::print_val(ostream &os, String space, bool print_decls)
{
    String separator;
    if (print_decls)
	separator = "\n";
    else
	separator = ", ";

    for (Pix p = first_var(); p; next_var(p), (void)(p && os << separator))
	var(p)->print_val(os, "", print_decls);
}
