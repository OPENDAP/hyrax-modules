
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

#include <iostream>
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

// This must only be called for simple structures!
void
AsciiStructure::print_header(ostream &os)
{
    for (Pix p = first_var(); p; next_var(p), (void)(p && os << ", "))
	if (var(p)->is_simple_type())
	    os << names.lookup(dynamic_cast<AsciiOutput*>(var(p))->get_full_name(), translate);
	else if (var(p)->type() == dods_structure_c)
	    dynamic_cast<AsciiStructure*>(var(p))->print_header(os);
	else
	    throw InternalErr(__FILE__, __LINE__,
"This method should only be called by instances for which `is_simple_structure' returns true.");
}

void
AsciiStructure::print_ascii(ostream &os, bool print_name) throw(InternalErr)
{
    if (is_linear()) {
	if (print_name) {
	    print_header(os);
	    os << endl;
	}
	
	for (Pix p = first_var(); p; next_var(p), (void)(p && os << ", "))
	    dynamic_cast<AsciiOutput*>(var(p))->print_ascii(os, false);
    }
    else {
	for (Pix p = first_var(); p; next_var(p)) {
	    dynamic_cast<AsciiOutput*>(var(p))->print_ascii(os, true);
	    // This line outputs an extra endl when print_ascii is called for
	    // nested structures because an endl is written for each member
	    // and then once for the structure itself. 9/14/2001 jhrg
	    os << endl;
	}
    }
}

// $Log: AsciiStructure.cc,v $
// Revision 1.6  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.5.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
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
