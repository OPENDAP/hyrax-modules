// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.cc,v $
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

static char rcsid[] __unused__ ={"$Id: name_map.cc,v 1.2 1996/11/23 05:17:39 jimg Exp $"};

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

String
name_map::lookup(String name) 
{
    for (Pix p = _names.first(); p; _names.next(p))
	if (name == _names(p).from)
	    return _names(p).to;
    return name;
}

void 
name_map::delete_all() 
{
    _names.clear();
}
