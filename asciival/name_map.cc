
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.cc,v $
// Revision 1.6  1999/07/24 00:10:07  jimg
// Repaired the munge function and removed SLList.
//
// Revision 1.5  1999/03/24 06:23:43  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.4  1997/02/06 20:44:51  jimg
// Fixed log messages
//
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

static char rcsid[] not_used = {"$Id: name_map.cc,v 1.6 1999/07/24 00:10:07 jimg Exp $"};

using namespace std;

#include <vector>
#include <string>

#include <Regex.h>

#include "escaping.h"
#include "name_map.h"

name_map::name_map(char *raw_equiv) 
{
    _names.push_back(name_equiv(raw_equiv));
}

name_map::name_map()
{
}

void 
name_map::add(char *raw_equiv) 
{
    name_equiv equiv(raw_equiv);
    _names.push_back(equiv);
}

static string
munge(string name)
{
    name = esc2underscore(name);
    return esc2underscore(name, "[^A-z0-9_]");
}

string
name_map::lookup(string name, const bool canonical_names = false) 
{
    // NEItor is the name_eqiv const interator. 7/23/99 jhrg
    for (NEItor p = _names.begin(); p != _names.end(); ++p)
	if (name == p->from) {
	    if (!canonical_names) {
		return p->to;
	    }
	    else {
		static Regex ident("[A-Za-z_][A-Za-z0-9_]*", 1);
		string tmp_n = p->to;
		if (ident.match(tmp_n.c_str(), tmp_n.length())) 
		    return munge(tmp_n);
		else
		    return tmp_n;
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
    _names.erase(_names.begin(), _names.end());
}

