
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt32. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

#include <assert.h>
#include <iostream.h>
#include <string>

#include "InternalErr.h"
#include "AsciiUInt32.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

UInt32 *
NewUInt32(const string &n)
{
    return new AsciiUInt32(n);
}

AsciiUInt32::AsciiUInt32(const string &n) : UInt32(n)
{
}

BaseType *
AsciiUInt32::ptr_duplicate()
{
    return new AsciiUInt32(*this);
}

bool
AsciiUInt32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
AsciiUInt32::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    UInt32::print_val(os, "", false);
}

// $Log: AsciiUInt32.cc,v $
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
