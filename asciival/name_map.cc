// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.cc,v $
// Revision 1.3  1997/01/10 06:50:30  jimg
// Added to lookup mfunc the ability to map non-alphanmerics to underscore.
//
// Revision 1.2  1996/11/23 05:17:39  jimg
// Added name_map() because g++ wants it.
//
// Revision 1.1  1996/11/22 23:52:01  jimg
// Created.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] __unused__ ={"$Id: name_map.cc,v 1.3 1997/01/10 06:50:30 jimg Exp $"};

#include <Pix.h>
#include <SLList.h>
#include <String.h>

#include "name_map.h"

name_map::name_map(char *raw_equiv) 
{
    _names.append(name_equiv(raw_equiv));
}

name_map::name_map()
{
}

void 
name_map::add(char *raw_equiv) 
{
    name_equiv equiv(raw_equiv);
    _names.append(equiv);
}

static String
munge(String name)
{
    static Regex bad("[^A-z0-9_]", 1);
    static Regex hex("%[0-9][0-9A-Fa-f]", 1);

    while (name.contains(hex))
	name.at(hex) = "_";

    while (name.contains(bad))
	name.at(bad) = "_";

    return name;
}

String
name_map::lookup(String name, const bool canonical_names = false) 
{
    for (Pix p = _names.first(); p; _names.next(p))
	if (name == _names(p).from) {
	    if (!canonical_names) {
		return _names(p).to;
	    }
	    else {
		String tmp_name = _names(p).to;
		if (tmp_name.matches(RXidentifier))
		    return tmp_name;
		else
		    return munge(tmp_name);
	    }
	}

    if (!canonical_names)
	return name;
    else
	return munge(name);
}

void 
name_map::delete_all() 
{
    _names.clear();
}
