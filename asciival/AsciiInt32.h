
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// AsciiInt32 interface. See AsciiByte.h for more info.
//
// 3/12/98 jhrg

#ifndef _asciiint32_h
#define _asciiint32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"
#include "AsciiOutput.h"

class AsciiInt32: public Int32, public AsciiOutput {
public:
    AsciiInt32(const string &n = (char *)0);
    virtual ~AsciiInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);
};

// $Log: AsciiInt32.h,v $
// Revision 1.4  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.3.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.3  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:23  jimg
// Added
//

#endif

