// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for WWWUrl type. See WWWByte.h
//
// 4/7/99 jhrg

// $Log: WWWUrl.h,v $
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#ifndef _WWWUrl_h
#define _WWWUrl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "WWWStr.h"

class WWWUrl: public WWWStr {
public:
    WWWUrl(const String &n = (char *)0);
    virtual ~WWWUrl() {}

    virtual BaseType *ptr_duplicate();
};

#endif

