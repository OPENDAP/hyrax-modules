// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for WWWUrl type. See WWWByte.h
//
// 4/7/99 jhrg

#ifndef _wwwurl_h
#define _wwwurl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "WWWStr.h"

class WWWUrl: public WWWStr {
public:
    WWWUrl(const string &n = (char *)0);
    virtual ~WWWUrl() {}

    virtual BaseType *ptr_duplicate();
};

// $Log: WWWUrl.h,v $
// Revision 1.3  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//

#endif

