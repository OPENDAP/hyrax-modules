// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for AsciiUrl type. See AsciiByte.h
//
// 3/12/98 jhrg

#ifndef _asciiurl_h
#define _asciiurl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "AsciiStr.h"

class AsciiUrl: public AsciiStr {
public:
    AsciiUrl(const string &n = (char *)0);
    virtual ~AsciiUrl() {}

    virtual BaseType *ptr_duplicate();
};

// $Log: AsciiUrl.h,v $
// Revision 1.3  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.2  1999/04/30 17:06:54  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.1  1998/03/13 21:25:26  jimg
// Added
//

#endif

