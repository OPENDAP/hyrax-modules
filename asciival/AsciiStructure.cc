
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class AsciiStructure. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_asciival.h"

#include <assert.h>
#include <iostream.h>
#include <string>

#include "InternalErr.h"
#include "AsciiStructure.h"
#include "AsciiSequence.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Structure *
NewStructure(const string &n)
{
    return new AsciiStructure(n);
}

BaseType *
AsciiStructure::ptr_duplicate()
{
    return new AsciiStructure(*this);
}

AsciiStructure::AsciiStructure(const string &n) : Structure(n)
{
}

AsciiStructure::~AsciiStructure()
{
}

// For this `Ascii' class, run the read mfunc for each of variables which
// comprise the structure. 

bool
AsciiStructure::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
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
AsciiStructure::print_val(ostream &os, string space, bool print_decls)
{
    string separator;
    if (print_decls)
	separator = "\n";
    else
	separator = ", ";

    for (Pix p = first_var(); p; next_var(p), (void)(p && os << separator))
	var(p)->print_val(os, "", print_decls);
}

void
AsciiStructure::print_all_vals(ostream &os, XDR *src, DDS *dds, string, bool)
{
    bool sequence_found = false;

    for (Pix p = first_var(); p; next_var(p)) {
	assert(var(p));

	switch (var(p)->type()) {
	  case dods_sequence_c:
	    (dynamic_cast<AsciiSequence *>
	     (var(p)))->print_all_vals(os, src, dds, "", false);
	    sequence_found = true;
	    break;
	  
	  case dods_structure_c:
	    (dynamic_cast<AsciiStructure *>
	     (var(p)))->print_all_vals(os, src, dds, "", false);
	    break;
	  
	  default:
	    // If a sequence was found, we still need to deserialize()
	    // remaining vars.
	    if(sequence_found)
		var(p)->deserialize(src, dds);
	    var(p)->print_val(os, "", false);
	    break;
	}
    }
}

// $Log: AsciiStructure.cc,v $
// Revision 1.5  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.4  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.3  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.2  1998/09/16 23:31:53  jimg
// Added print_all_vals().
//
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//
