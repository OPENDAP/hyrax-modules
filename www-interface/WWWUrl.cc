
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for WWWUrl. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWUrl.cc,v 1.3 2000/10/03 20:07:21 jimg Exp $"};

#include <string>

#include "WWWUrl.h"

WWWUrl *
NewUrl(const string &n)
{
    return new WWWUrl(n);
}

WWWUrl::WWWUrl(const string &n) : WWWStr(n)
{
}

BaseType *
WWWUrl::ptr_duplicate()
{
    return new WWWUrl(*this);
}

// $Log: WWWUrl.cc,v $
// Revision 1.3  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//
