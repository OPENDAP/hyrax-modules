
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

// Implementation for the class WWWStructure. See WWWByte.cc
//
// 4/7/99 jhrg

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_www_int.h"

static char rcsid[] not_used = {"$Id: WWWSequence.cc,v 1.5 2003/01/27 23:53:54 jimg Exp $"};

#include <assert.h>

#include <iostream.h>

#include <Pix.h>
#include <string>

#include "DAS.h"
#include "InternalErr.h"

#include "WWWSequence.h"
#include "WWWOutput.h"

extern DAS global_das;
extern WWWOutput wo;

Sequence *
NewSequence(const string &n)
{
    return new WWWSequence(n);
}

BaseType *
WWWSequence::ptr_duplicate()
{
    return new WWWSequence(*this);
}

WWWSequence::WWWSequence(const string &n) : Sequence(n)
{
}

WWWSequence::~WWWSequence()
{
}

bool 
WWWSequence::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Called unimplemented read method");
}

int
WWWSequence::length()
{
    return -1;
}

bool
WWWSequence::is_simple_sequence()
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->type() == dods_sequence_c) {
	    if (!dynamic_cast<WWWSequence *>(var(p))->is_simple_sequence())
		return false;
	}
	else {
	    if (!var(p)->is_simple_type())
		return false;
	}
    }

    return true;
}

// As is the case with geturl, use print_all_vals to print all the values of
// a sequence. 

void 
WWWSequence::print_val(ostream &os, string space, bool print_decls)
{
    os << "<b>Sequence " << name() << "</b><br>\n";
    os << "<dl><dd>\n";

    for (Pix p = first_var(); p; next_var(p)) {
	var(p)->print_val(os, "", print_decls);
	wo.write_variable_attributes(var(p), global_das);
	os << "<p><p>\n";
    }

    os << "</dd></dl>\n";
}

// $Log: WWWSequence.cc,v $
// Revision 1.5  2003/01/27 23:53:54  jimg
// Merged with release-3-2-7.
//
// Revision 1.4.2.1  2002/09/05 22:27:29  pwest
// Removed includes to SLList and DLList. These are not necessary and no longer
// supported.
//
// Revision 1.4  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//
// Revision 1.3  2000/10/02 22:42:44  jimg
// Modified the read method to match the new definition in the dap
//
// Revision 1.2  1999/05/09 04:14:51  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:04  jimg
// First version
//
