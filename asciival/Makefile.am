
# Automake file for asciival
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign dejagnu

# These are not used by automak but are often useful for certain types of
# debugging.
CXXFLAGS_DEBUG = -g3 -O0 -Wall -fno-defer-pop
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

sbin_PROGRAMS = asciival

asciival_SOURCES = AsciiArray.cc AsciiInt16.cc AsciiStructure.cc	     \
		AsciiInt32.cc AsciiUInt16.cc AsciiByte.cc AsciiOutput.cc     \
		AsciiUInt32.cc AsciiFloat32.cc AsciiUrl.cc AsciiFloat64.cc   \
		AsciiSequence.cc ascii_val.cc AsciiGrid.cc AsciiStr.cc	     \
		name_map.cc AsciiArray.h AsciiInt32.h AsciiUInt32.h	     \
		AsciiByte.h AsciiOutput.h AsciiUrl.h AsciiFloat32.h	     \
		AsciiSequence.h name_map.h AsciiFloat64.h AsciiStr.h	     \
		AsciiGrid.h AsciiStructure.h AsciiInt16.h AsciiUInt16.h	     \
		config_asciival.h AsciiOutputFactory.cc AsciiOutputFactory.h

UNIT_TESTS = AsciiArrayTest AsciiOutputTest

# Listing the unit test programs in TESTS causes the programs to be run as
# part of the check: target; listing them again under check_PROGRAMS causes
# them to be built.
TESTS = $(UNIT_TESTS)
check_PROGRAMS = $(UNIT_TESTS)

AsciiArrayTest_SOURCES = AsciiArrayTest.cc
AsciiOutputTest_SOURCES = AsciiOutputTest.cc

ASCIIOBJS = AsciiByte.o AsciiInt32.o AsciiFloat64.o AsciiStr.o AsciiUrl.o     \
	AsciiArray.o AsciiStructure.o AsciiSequence.o AsciiGrid.o	      \
	AsciiUInt32.o AsciiInt16.o AsciiUInt16.o AsciiFloat32.o AsciiOutput.o \
	AsciiOutputFactory.o

AsciiArrayTest_LDADD = $(ASCIIOBJS) name_map.o -lcppunit
AsciiOutputTest_LDADD = $(ASCIIOBJS) name_map.o -lcppunit

DEJATOOL = asciival
RUNTESTDEFAULTFLAGS = --tool $$tool --srcdir testsuite

EXTRA_DIST = ChangeLog testsuite 

