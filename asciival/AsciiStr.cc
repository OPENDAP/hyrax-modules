
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiStr. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiStr.cc,v $
// Revision 1.1  1998/03/13 21:25:18  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_writeval.h"

#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <iostream.h>
#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "AsciiStr.h"
#include "name_map.h"

extern bool translate;
extern name_map names;

Str *
NewStr(const String &n)
{
    return new AsciiStr(n);
}

AsciiStr::AsciiStr(const String &n) : Str(n)
{
}

BaseType *
AsciiStr::ptr_duplicate()
{
    return new AsciiStr(*this);
}

bool
AsciiStr::read(const String &, int &)
{
    assert(false);
    return false;
}

void 
AsciiStr::print_val(ostream &os, String, bool print_decl_p)
{
    if (print_decl_p)
	os << names.lookup(name(), translate) << ", ";
    Str::print_val(os, "", false);
}
