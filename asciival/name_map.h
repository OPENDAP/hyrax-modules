
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: name_map.h,v $
// Revision 1.1  1996/11/22 23:50:21  jimg
// Created.
//

#ifndef _NAME_MAP_H
#define _NAME_MAP_H

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
	~name_equiv() {
	}
    }

    SLList<name_equiv> _names;

public:
    name_map(char *raw_equiv);

    void add(char *raw_equiv);

    String lookup(String name);

    void delete_all();
}

#endif // _NAME_MAP_H
