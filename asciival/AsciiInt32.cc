
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt32. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiInt32.cc,v $
// Revision 1.1  1998/03/13 21:25:17  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <iostream.h>

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiInt32.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Int32 *
NewInt32(const String &n)
{
    return new AsciiInt32(n);
}

AsciiInt32::AsciiInt32(const String &n) : Int32(n)
{
}

BaseType *
AsciiInt32::ptr_duplicate()
{
    return new AsciiInt32(*this);
}

bool
AsciiInt32::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiInt32::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Int32::print_val(os, "", false);
}
