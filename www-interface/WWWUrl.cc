
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of www_int, software which returns an HTML form which
// can be used to build a URL to access data from a DAP data server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// asciival is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
// 
// asciival is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
// 
// You should have received a copy of the GNU General Public License along
// with GCC; see the file COPYING. If not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for WWWUrl. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef __GNUG__
//#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWUrl.cc,v 1.5 2003/12/08 18:08:02 edavis Exp $"};

#include <string>

#include "WWWUrl.h"

WWWUrl *
NewUrl(const string &n)
{
    return new WWWUrl(n);
}

WWWUrl::WWWUrl(const string &n) : Url(n)
{
}

BaseType *
WWWUrl::ptr_duplicate()
{
    return new WWWUrl(*this);
}

// $Log: WWWUrl.cc,v $
// Revision 1.5  2003/12/08 18:08:02  edavis
// Merge release-3-4 into trunk
//
// Revision 1.4  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.3  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:05  jimg
// First version
//
