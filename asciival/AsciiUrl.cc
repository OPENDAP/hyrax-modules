
// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiUrl. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

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

// $Log: AsciiUrl.cc,v $
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/03/24 06:23:42  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.1  1998/03/13 21:25:20  jimg
// Added
