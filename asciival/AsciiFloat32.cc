
// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiFloat32. See AsciiByte.cc
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
#include "AsciiFloat32.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Float32 *
NewFloat32(const string &n)
{
    return new AsciiFloat32(n);
}

AsciiFloat32::AsciiFloat32(const string &n) : Float32(n)
{
}

BaseType *
AsciiFloat32::ptr_duplicate()
{
    return new AsciiFloat32(*this);
}
 
bool
AsciiFloat32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

void 
AsciiFloat32::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Float32::print_val(os, "", false);
}

// $Log: AsciiFloat32.cc,v $
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:53  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/04/30 17:06:53  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1999/03/29 21:22:55  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:15  jimg
// Added
//
