
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.cc,v $
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

static char rcsid[] not_used = {"$Id: name_map.cc,v 1.5 1999/03/24 06:23:43 brent Exp $"};

#include <Pix.h>
#include <SLList.h>
#include <string>

#include "name_map.h"

extern "C" {
#include "rx.h"
}

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

static string
munge(string name)
{

	regex_t bad, hex;		// compiled regular expressions 
	regmatch_t p_match;		// search results 
	char target[260];		// temporay buffer where we work 
	int i, j, error, len, offset = 0;	// assorted counters 
	int org_len;

	org_len = name.size();		 
	name.copy(target, org_len);;	// C++ to C copy 
	target[org_len] = 0;		// append a NULL 

	regcomp(&hex, "%[0-9][0-9A-Fa-f]", REG_EXTENDED);
	regcomp(&bad, "[^A-z0-9_]", REG_EXTENDED);

	// substitute the hex characters with an '_' 
	error = regexec (&hex, &target[0], 1, &p_match, 0);
	while ((error == 0) && (offset < org_len)) {     
		// while match found -- the match is always some hex number 
		len = p_match.rm_eo - p_match.rm_so;
		for (i = offset + p_match.rm_so, j = 0; j < len; ++j, ++i)
			target[i] = '_';
		offset += p_match.rm_eo;
		error = regexec (&hex, target + offset, 1, 
			&p_match, REG_NOTBOL);
	}

	// substitute the non-bad characters with an '_' 
	offset = 0;
	error = regexec (&bad, &target[0], 1, &p_match, 0);
	while ((error == 0) && (offset < org_len)) {     
		// while match found -- the match is always a single char 
		target[offset + p_match.rm_so] = '_';
		offset += p_match.rm_eo;
		error = regexec (&bad, target + offset, 1, 
			&p_match, REG_NOTBOL);
	}
	regfree(&bad);	// let go of the space
	regfree(&hex);
	// put result back in string object -- leave the NULL behind
	name.replace(0, org_len, target);	
	
	return name;
}

string
name_map::lookup(string name, const bool canonical_names = false) 
{
    regex_t ident;
    regmatch_t pm;
    int rc, len;
    char target[260];

    for (Pix p = _names.first(); p; _names.next(p))
	if (name == _names(p).from) {
	    if (!canonical_names) {
		return _names(p).to;
	    }
	    else {
		string tmp_name = _names(p).to;
		// don't need the RE, except for this case, so do it all here
		len = tmp_name.size();
		tmp_name.copy(target, len);
		target[len] = 0;
		regcomp(&ident, "[A-Za-z_][A-Za-z0-9_]*", REG_EXTENDED);
		rc = regexec(&ident, &target[0], 1, &pm, 0);
		regfree(&ident);
		if (rc == 0)
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

