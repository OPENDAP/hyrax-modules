
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiByte. See the comments in AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiByte.cc,v $
// Revision 1.4  1999/07/28 23:00:53  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.3  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.2  1998/03/16 19:47:36  jimg
// Fixed lame comments.
//
// Revision 1.1  1998/03/13 21:25:15  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

// The NewByte `helper function' creates a pointer to an AsciiByte and
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

#include "config_asciival.h"

#include <stdio.h>
#include <assert.h>

#include <iostream.h>
#include <string>

#include "AsciiByte.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Byte *
NewByte(const string &n)
{
    return new AsciiByte(n);
}

AsciiByte::AsciiByte(const string &n) : Byte(n)
{
}

BaseType *
AsciiByte::ptr_duplicate()
{
    return new AsciiByte(*this);
}

bool
AsciiByte::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
AsciiByte::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Byte::print_val(os, "", false);
}
