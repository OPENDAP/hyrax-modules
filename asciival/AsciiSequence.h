
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the class AsciiSequence. See AsciiByte.h
//
// 3/12/98 jhrg

// $Log: AsciiSequence.h,v $
// Revision 1.1  1998/03/13 21:25:24  jimg
// Added
//

#ifndef _AsciiSequence_h
#define _AsciiSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"

class AsciiSequence: public Sequence {
private:
    bool is_simple_sequence();
    void print_header(ostream &os);

public:
    AsciiSequence(const String &n = (char *)0);
    virtual ~AsciiSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    virtual int length();

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual void print_all_vals(ostream &os, XDR *src, DDS *dds, 
				String space = "", bool print_decl_p = true);
};

#endif
