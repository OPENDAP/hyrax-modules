
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiByte. See the comments in AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiByte.cc,v $
// Revision 1.1  1998/03/13 21:25:15  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

// The NewByte `helper function' creates a pointer to the a AsciiByte and
// returns that pointer. It takes the same arguments as the class's ctor. If
// any of the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instnace of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these fuctions (in util.h) should *not* need
// changing. 

#include <stdio.h>
#include <assert.h>

#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiByte.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Byte *
NewByte(const String &n)
{
    return new AsciiByte(n);
}

AsciiByte::AsciiByte(const String &n) : Byte(n)
{
}

BaseType *
AsciiByte::ptr_duplicate()
{
    return new AsciiByte(*this);
}

bool
AsciiByte::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiByte::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Byte::print_val(os, "", false);
}
