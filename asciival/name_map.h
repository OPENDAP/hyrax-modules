
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.h,v $
// Revision 1.3  1997/01/10 06:50:32  jimg
// Added to lookup mfunc the ability to map non-alphanmerics to underscore.
//
// Revision 1.2  1996/11/23 05:17:56  jimg
// Added name_map().
//
// Revision 1.1  1996/11/22 23:50:21  jimg
// Created.
//

#ifndef _NAME_MAP_H
#define _NAME_MAP_H

#ifdef __GNUG__
#pragma interface
#endif

/// Map names using a simple thesaurus.
/** This class can be used to build up a simple thesaurus which maps names from
    one string to another. The thesaurus is built by calling the #add# mfunc
    with a string of the form <source>:<dest> where <source> and <dest> are
    string literals. Once the thesaurus is built, the #lookup# mfunc can be
    used to get the equivalent of any string in the thesaurus. 

    An additional feature, this class can canonicalize an identifier (remove
    non-alphanumeric characters) when performing the lookup operation. This
    is performed as a separate step from the thesaurus scan so words with no
    entry in the thesaurus can still be canonicalized.

    You can interleave calls to #add# and calls to #lookup#.
*/

class name_map {
private:
    struct name_equiv {
	String from;
	String to;
	name_equiv(char *raw_equiv) {
	    String t = raw_equiv;
	    from = t.before(":");
	    to = t.after(":");
	}
	name_equiv() {
	}
	~name_equiv() {
	}
    };

    SLList<name_equiv> _names;

public:
    /// Create a new instance of the thesaurus.
    /** Create a new instance of the thesaurus and add the first equivalence
        string to it.
    */
    name_map(char *raw_equiv);

    /// Create an empty thesaurus.
    name_map();

    /// Add a new entry to the thesaurus.
    /** Add a new entry to the thesaurus. The form of the new entry is
        <source>:<dest>.

	Returns: void
    */
    void add(char *raw_equiv);

    /// Lookup a work in the thesaurus.
    /** Lookup a word in the thesaurus. If the name appears in the thesaurus,
        return its equivalent. If #canonical_names# is true, pass the string
	#name# or its equivalent through a filter to replacing all characters
	that are not alphanumerics with the underscore. Sequences matching
	`%[0-9][0-9a-fA-F]' are considered to be hex escape codes and are
	replaced by a single underscore.

	Returns: the string #name#, it equivalent or its canonicalized
	equivalent.
    */
    String lookup(String name, const bool canonical_names = false);

    /// Delete all the entries from the thesaurus.
    void delete_all();
};

#endif // _NAME_MAP_H
