
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the class AsciiStructure. See AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiStructure.h,v $
// Revision 1.2  1998/09/16 23:31:29  jimg
// Added print_all_vals().
//
// Revision 1.1  1998/03/13 21:25:25  jimg
// Added
//

#ifndef _AsciiStructure_h
#define _AsciiStructure_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Structure.h"

class AsciiStructure: public Structure {
private:
    bool is_simple_structure();
    void print_header(ostream &os);

public:
    AsciiStructure(const String &n = (char *)0);
    virtual ~AsciiStructure();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual void print_all_vals(ostream &os, XDR *src, DDS *dds, 
				String space = "", bool print_decl_p = true);
};

#endif
