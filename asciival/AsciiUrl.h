// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for AsciiUrl type. See AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiUrl.h,v $
// Revision 1.1  1998/03/13 21:25:26  jimg
// Added
//

#ifndef _AsciiUrl_h
#define _AsciiUrl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "AsciiStr.h"

class AsciiUrl: public AsciiStr {
public:
    AsciiUrl(const String &n = (char *)0);
    virtual ~AsciiUrl() {}

    virtual BaseType *ptr_duplicate();
};

#endif

