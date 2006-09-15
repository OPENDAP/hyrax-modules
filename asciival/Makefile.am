
# Automake file for asciival
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign dejagnu

AM_CPPFLAGS = -I$(top_srcdir)

# These are not used by automake but are often useful for certain types of
# debugging.
CXXFLAGS_DEBUG = -g3 -O0 -Wall -fno-defer-pop
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

DIST_SUBDIRS = unit-tests

bin_PROGRAMS = dap_asciival

# lib_LTLIBRARIES = libascii_handler.la

if OPENDAPSERVER
    lib_besdir=$(BES_MODULE_DIR)
    lib_bes_LTLIBRARIES = libascii_module.la
    libascii_module_la_SOURCES = $(ASCII_SOURCES) $(BES_SOURCES)
    libascii_module_la_CPPFLAGS = $(BES_CPPFLAGS)
    libascii_module_la_LDFLAGS = -avoid-version -module 
    libascii_module_la_LIBADD = $(BES_LIBS)
endif

# libascii_handler_la_SOURCES = 

dap_asciival_SOURCES = $(ASCII_SOURCES) ascii_val.cc
# dap_asciival_LDADD = libascii_handler.la

ASCII_SOURCES = AsciiArray.cc AsciiInt16.cc AsciiStructure.cc	     \
		AsciiInt32.cc AsciiUInt16.cc AsciiByte.cc AsciiOutput.cc     \
		AsciiUInt32.cc AsciiFloat32.cc AsciiUrl.cc AsciiFloat64.cc   \
		AsciiSequence.cc  AsciiGrid.cc AsciiStr.cc AsciiArray.h \
		AsciiInt32.h AsciiUInt32.h AsciiByte.h AsciiOutput.h \
		AsciiUrl.h AsciiFloat32.h AsciiSequence.h AsciiFloat64.h \
		AsciiStr.h AsciiGrid.h AsciiStructure.h AsciiInt16.h \
		AsciiUInt16.h AsciiOutputFactory.cc AsciiOutputFactory.h \
		get_ascii.cc get_ascii.h name_map.cc name_map.h \
		config_asciival.h 

BES_SOURCES = BESAsciiModule.cc BESAsciiResponseHandler.cc BESAsciiTransmit.cc \
		BESAsciiModule.h BESAsciiResponseHandler.h BESAsciiTransmit.h

# ASCIIOBJS = AsciiByte.o AsciiInt32.o AsciiFloat64.o AsciiStr.o AsciiUrl.o     \
#	AsciiArray.o AsciiStructure.o AsciiSequence.o AsciiGrid.o	      \
#	AsciiUInt32.o AsciiInt16.o AsciiUInt16.o AsciiFloat32.o AsciiOutput.o \
#	AsciiOutputFactory.o

DEJATOOL = dap_asciival
RUNTESTDEFAULTFLAGS = --tool $$tool --srcdir testsuite

EXTRA_DIST = ChangeLog testsuite common_tests.exp

CLEANFILES = *~ *.log *.sum
