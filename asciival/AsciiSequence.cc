
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
#include "AsciiSequence.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Sequence *
NewSequence(const string &n)
{
    return new AsciiSequence(n);
}

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

bool
AsciiSequence::is_simple_sequence()
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->type() == dods_sequence_c) {
	    if (!((AsciiSequence *)var(p))->is_simple_sequence())
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
AsciiSequence::print_header(ostream &os)
{
    for (Pix p = first_var(); p; next_var(p), (void)(p && os << ", "))
	if (var(p)->is_simple_type())
	    os << names.lookup(var(p)->name(), translate);
	else if (var(p)->type() == dods_sequence_c)
	    ((AsciiSequence *)var(p))->print_header(os);
}

// As is the case with geturl, use print_all_vals to print all the values of
// a sequence. 

void 
AsciiSequence::print_val(ostream &os, string space, bool print_decls)
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
AsciiSequence::print_all_vals(ostream &os, XDR *src, DDS *dds, string, bool)
{
    bool print_decls;

    if (is_simple_sequence()) {
	print_header(os);
	os << endl;
	print_decls = false;
    }
    else {
	print_decls = true;
    }
	
    print_val(os, "", print_decls);
    while (deserialize(src, dds)) {
	os << endl;
	print_val(os, "", print_decls);
    }
}

// $Log: AsciiSequence.cc,v $
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

