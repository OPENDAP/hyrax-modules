
// Tests for the DataDDS class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DDS.h"
#include "AsciiOutput.h"

class AsciiOutputTest : public TestCase {
private:
    DDS *dds;

public: 
    AsciiOutputTest (string name) : TestCase (name) {}

    void setUp() {
	dds = new DDS("ascii_output_test");
	dds->parse("testsuite/AsciiOutputTest1.dds");
    }

    void tearDown() {
	delete dds; dds = 0;
    }

    void test_get_full_name() {
	assert(dynamic_cast<AsciiOutput*>(dds->var("a"))->get_full_name() == "a");
	cerr << "full name: " 
	     << dynamic_cast<AsciiOutput*>(dds->var("e.c"))->get_full_name()
	     << endl;

	assert(dynamic_cast<AsciiOutput*>(dds->var("e.c"))->get_full_name() == "e.c");
	assert(dynamic_cast<AsciiOutput*>(dds->var("f.c"))->get_full_name() == "f.c");
	assert(dynamic_cast<AsciiOutput*>(dds->var("g.y"))->get_full_name() == "g.y");
	assert(dynamic_cast<AsciiOutput*>(dds->var("k.h.i"))->get_full_name() == "k.h.i");
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("AsciiOutputTest");
	s->addTest(new TestCaller<AsciiOutputTest>
		   ("test_get_full_name",
		    &AsciiOutputTest::test_get_full_name));

	return s;
    }
};




