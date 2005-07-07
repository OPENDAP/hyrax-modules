
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an ASCII
// representation of the data read from a DAP server.

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
 
// (c) COPYRIGHT URI/MIT 1996,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

#ifndef _name_map_h
#define _name_map_h

#ifdef __GNUG__
//#pragma interface
#endif

#include <vector>
#include <string>

using std::vector;
using std::string;

/** This class can be used to build up a simple thesaurus which maps names
    from one string to another. The thesaurus is built by calling the #add#
    mfunc with a string of the form <source>:<dest> where <source> and <dest>
    are string literals. Once the thesaurus is built, the #lookup# mfunc can
    be used to get the equivalent of any string in the thesaurus.

    As an additional feature, this class can canonicalize an identifier
    (remove non-alphanumeric characters) when performing the lookup
    operation. This is performed as a separate step from the thesaurus scan
    so words with no entry in the thesaurus will still be canonicalized.

    You can interleave calls to #add# and calls to #lookup#. 

    @memo Map names using a simple thesaurus.
    @author jhrg */

class name_map {
private:
    struct name_equiv {
    	int colon;
	string from;
	string to;
	name_equiv(char *raw_equiv) {
	    string t = raw_equiv;
	    colon = t.find(":");
	    from = t.substr(0,colon);
	    to = t.substr(colon+1, t.size());
	}
	name_equiv() {
	}
	~name_equiv() {
	}
    };

    vector<name_equiv> _names;
    typedef vector<name_equiv>::iterator NEItor;

public:
    /** Create a new instance of the thesaurus and add the first equivalence
        string to it. 
	
	@memo Create a new instance of the thesaurus. */
    name_map(char *raw_equiv);

    /** Create an empty thesaurus. */
    name_map();

    /** Add a new entry to the thesaurus. The form of the new entry is
        <source>:<dest>. */
    void add(char *raw_equiv);

    /** Lookup a word in the thesaurus. If the name appears in the thesaurus,
        return its equivalent. If #canonical_names# is true, pass the string
	#name# or its equivalent through a filter to replace all characters
	that are not alphanumerics with the underscore. Sequences matching
	`%[0-9][0-9a-fA-F]' are considered to be hex escape codes and are
	replaced by a single underscore.

	@return The string #name#, its equivalent or its canonicalized
	equivalent. */
    string lookup(string name, const bool canonical_names = false);

    /** Delete all the entries from the thesaurus. */
    void delete_all();
};

// $Log: name_map.h,v $
// Revision 1.12  2004/07/08 22:18:25  jimg
// Merged with release-3-4-3FCS
//
// Revision 1.11.4.3  2004/07/05 03:12:10  rmorris
// Use "using std::*" in the standard way.
//
// Revision 1.11.4.2  2003/07/11 04:28:18  jimg
// Boldly (ahem) removed the GNUG interface/implementation //#pragma.
// compared object sizes; there's a little difference, but it looks like a
// draw as to which is bigger.
//
// Revision 1.11.4.1  2003/06/26 07:38:59  rmorris
// #ifdef'd out //#pragma interface directives under OS X.  They cause problems
// with the dynamic typing system under Jaguar.
//
// Revision 1.11  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//
// Revision 1.10  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.9.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.9  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.8  1999/07/24 00:10:07  jimg
// Repaired the munge function and removed SLList.
//
// Revision 1.7  1999/04/30 17:06:58  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.6  1999/03/24 06:40:48  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.5.18.1  1999/03/30 20:26:02  jimg
// Added include of SLList
//
// Revision 1.5  1998/03/13 21:24:26  jimg
// Fixed up comments
//
// Revision 1.4  1997/02/06 20:44:52  jimg
// Fixed log messages
//
// Revision 1.3  1997/01/10 06:50:32  jimg
// Added to lookup mfunc the ability to map non-alphanmerics to underscore.
//
// Revision 1.2  1996/11/23 05:17:56  jimg
// Added name_map().
//
// Revision 1.1  1996/11/22 23:50:21  jimg
// Created.
//

#endif // _name_map_h
