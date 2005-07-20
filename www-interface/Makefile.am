
# Automake file for www-interface
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -DDEFAULT_BASETYPE_FACTORY

# These are not used by automak but are often useful for certain types of
# debugging.
CXXFLAGS_DEBUG = -g3 -O0 -Wall -fno-defer-pop
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

sbin_PROGRAMS = www_int

www_int_SOURCES = WWWArray.cc WWWFloat64.cc WWWInt32.cc WWWSequence.cc	     \
	WWWUInt16.cc WWWByte.cc WWWGrid.cc www_int.cc WWWStr.cc WWWUInt32.cc \
	WWWFloat32.cc WWWInt16.cc WWWOutput.cc WWWStructure.cc WWWUrl.cc     \
	WWWFloat32.h WWWInt16.h WWWSequence.h WWWUInt16.h WWWArray.h	     \
	WWWFloat64.h WWWInt32.h WWWStr.h WWWUInt32.h WWWByte.h WWWGrid.h     \
	WWWOutput.h WWWStructure.h WWWUrl.h javascript.h config_www_int.h

dist_pkgdata_DATA = dods_form_help.html

EXTRA_DIST = ChangeLog js2h.pl www.js

javascript.h: www.js
	./js2h.pl $< > $@

www_int.o: javascript.h