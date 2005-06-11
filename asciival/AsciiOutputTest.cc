
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

// Tests for the DataDDS class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <DDS.h>

#include "AsciiOutput.h"
#include "name_map.h"

// These globals are defined in ascii_val.cc and are needed by the Ascii*
// classes. This code has to be linked with those so that the Ascii*
// specializations of Byte, ..., Grid will be instantiated by DDS when it
// parses a .dds file. Each of those subclasses is a child of AsciiOutput in
// addition to its regular lineage. This test code depends on being able to
// cast each variable to an AsciiOutput object. 01/24/03 jhrg
name_map names;
bool translate = false;

using namespace CppUnit;

class AsciiOutputTest : public TestFixture {
private:
    DDS *dds;

public: 
    AsciiOutputTest() {}
    ~AsciiOutputTest() {}

    void setUp() {
	dds = new DDS("ascii_output_test");
	dds->parse("testsuite/AsciiOutputTest1.dds");
    }

    void tearDown() {
	delete dds; dds = 0;
    }

    CPPUNIT_TEST_SUITE(AsciiOutputTest);

    CPPUNIT_TEST(test_get_full_name);

    CPPUNIT_TEST_SUITE_END();

    void test_get_full_name() {
	CPPUNIT_ASSERT(dynamic_cast<AsciiOutput*>(dds->var("a"))->get_full_name() == "a");
	cerr << "full name: " 
	     << dynamic_cast<AsciiOutput*>(dds->var("e.c"))->get_full_name()
	     << endl;

	CPPUNIT_ASSERT(dynamic_cast<AsciiOutput*>(dds->var("e.c"))->get_full_name() == "e.c");
	CPPUNIT_ASSERT(dynamic_cast<AsciiOutput*>(dds->var("f.c"))->get_full_name() == "f.c");
	CPPUNIT_ASSERT(dynamic_cast<AsciiOutput*>(dds->var("g.y"))->get_full_name() == "g.y");
	CPPUNIT_ASSERT(dynamic_cast<AsciiOutput*>(dds->var("k.h.i"))->get_full_name() == "k.h.i");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AsciiOutputTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: AsciiOutputTest.cc,v $
// Revision 1.3  2003/01/27 19:38:23  jimg
// Updated the copyright information.
// Merged with release-3-2-6.
//

