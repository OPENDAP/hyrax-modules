
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiUrl. See AsciiByte.cc
//
// 3/12/98 jhrg

// $Log: AsciiUrl.cc,v $
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:20  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <string>

#include "AsciiUrl.h"

AsciiUrl *
NewUrl(const string &n)
{
    return new AsciiUrl(n);
}

AsciiUrl::AsciiUrl(const string &n) : AsciiStr(n)
{
}

BaseType *
AsciiUrl::ptr_duplicate()
{
    return new AsciiUrl(*this);
}
