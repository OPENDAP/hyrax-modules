
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiUrl. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiUrl.cc,v $
// Revision 1.1  1998/03/13 21:25:20  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <String.h>

#include "AsciiUrl.h"

AsciiUrl *
NewUrl(const String &n)
{
    return new AsciiUrl(n);
}

AsciiUrl::AsciiUrl(const String &n) : AsciiStr(n)
{
}

BaseType *
AsciiUrl::ptr_duplicate()
{
    return new AsciiUrl(*this);
}
