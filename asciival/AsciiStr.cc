
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiStr. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiStr.cc,v $
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:18  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

#include <assert.h>

#include <iostream.h>
#include <string>

#include "AsciiStr.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Str *
NewStr(const string &n)
{
    return new AsciiStr(n);
}

AsciiStr::AsciiStr(const string &n) : Str(n)
{
}

BaseType *
AsciiStr::ptr_duplicate()
{
    return new AsciiStr(*this);
}

bool
AsciiStr::read(const string &, int &)
{
    assert(false);
    return false;
}

void 
AsciiStr::print_val(ostream &os, string, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Str::print_val(os, "", false);
}
