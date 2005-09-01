
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([dap-server], [3.5.1], [dods-tech@unidata.ucar.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([nph-dods.in])
AM_CONFIG_HEADER([config.h])

AC_DEFINE([DAP_VERSION], ["2.0"], [What version of the data server is this?])

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

# Until further notice OS X port doesn't get -gstabs because of an
# apparent bug in their gcc 3.3 clone (a beta).  If you see this after
# 1/1/2005, you can probably remove it.
case "${host_cpu}:${host_vendor}:${host_os}" in
     powerpc:apple:netbsd* | alphaev56:dec:osf5.* | powerpc:apple:darwin* )
       CFLAGS=${CFLAGS-"-O2 $CFLAGS"}
       CXXFLAGS=${CXXFLAGS-"-O2 $CXXFLAGS"}
       break;;
    *:*:*)
       CFLAGS=${CFLAGS-"-gstabs -O2 $CFLAGS"}
       CXXFLAGS=${CXXFLAGS-"-gstabs -O2 $CXXFLAGS"}
       break;;
esac    

dnl Default to g++

CXX=${CXX-g++}
AC_MSG_RESULT(Setting CXX to $CXX)
AC_SUBST(CXX)

CFLAGS=${CFLAGS-"-gstabs -O2 $CFLAGS"}
CXXFLAGS=${CXXFLAGS-"-gstabs -O2 $CXXFLAGS"}

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AM_PROG_LEX
AC_PROG_RANLIB
AC_PROG_YACC
AC_PROG_MAKE_SET

DODS_PROG_PERL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([putenv strtol])

dnl Checks for specific libraries

AC_CHECK_LIBDAP([3.5.0],
 [
  LIBS="$LIBS $DAP_LIBS"
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.

AC_PATH_PROG([CURL], [curl], [*** Where is curl? ***],
	         [/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/sbin])

OPENDAP_DEBUG_OPTION

dnl autoheader macros; tack some text at the top and bottom of config.h.in
AH_TOP([#ifndef _config_h
#define _config_h])

AH_BOTTOM([/* Shorthand for gcc's unused attribute feature */
#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif /* __GNUG__ || __GNUC__ */

#endif /* _config_h */])

AC_CONFIG_FILES([dap-server.rc 
		 Makefile
                 asciival/Makefile
		 asciival/unit-tests/Makefile
                 www-interface/Makefile])

AC_CONFIG_FILES([nph-dods], [chmod +x nph-dods])
AC_CONFIG_FILES([www-interface/js2h.pl], [chmod +x www-interface/js2h.pl])

AC_OUTPUT
