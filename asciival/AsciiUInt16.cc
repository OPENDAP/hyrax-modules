
// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for AsciiInt16. See AsciiByte.cc
//
// 3/12/98 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_asciival.h"

#include <string>

#include "InternalErr.h"
#include "AsciiUInt16.h"

UInt16 *
NewUInt16(const string &n)
{
    return new AsciiUInt16(n);
}

AsciiUInt16::AsciiUInt16(const string &n) : UInt16(n)
{
}

BaseType *
AsciiUInt16::ptr_duplicate()
{
    return new AsciiUInt16(*this);
}

bool
AsciiUInt16::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

// $Log: AsciiUInt16.cc,v $
// Revision 1.5  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.4.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.4  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.3  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1999/03/29 21:22:54  jimg
// Added
//
// Revision 1.1  1998/03/13 21:25:19  jimg
// Added
//
