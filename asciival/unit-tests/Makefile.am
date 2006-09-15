
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I../
AM_CXXFLAGS = -I$(top_srcdir)/GNU

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.' The dejagnu_driver.sh 
# runs all the dejagnu test drivers by calling runtest 'by hand.'
TESTS = $(UNIT_TESTS)

EXTRA_DIST = testsuite

CLEANFILES = 

############################################################################
# Unit Tests
#

# The object files from asciival are needed to link the unit tests
ASCIIOBJS = ../AsciiByte.o ../AsciiInt32.o ../AsciiFloat64.o ../AsciiStr.o   \
	../AsciiUrl.o ../AsciiArray.o ../AsciiStructure.o ../AsciiSequence.o \
	../AsciiGrid.o ../AsciiUInt32.o ../AsciiInt16.o ../AsciiUInt16.o     \
	../AsciiFloat32.o ../AsciiOutput.o ../AsciiOutputFactory.o	     \
	../name_map.o ../get_ascii.o

UNIT_TESTS = AsciiArrayTest AsciiOutputTest

AsciiArrayTest_SOURCES = AsciiArrayTest.cc
AsciiOutputTest_SOURCES = AsciiOutputTest.cc

AsciiArrayTest_LDADD = $(ASCIIOBJS) -lcppunit
AsciiOutputTest_LDADD = $(ASCIIOBJS) -lcppunit

