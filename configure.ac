
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([dap-server], [3.5.0], [dods-tech@unidata.ucar.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([nph-dods.in])

AC_DEFINE([DAP_VERSION], [$PACKAGE_VERSION],
		  [What version of the data server is this?])

AC_PROG_INSTALL
AC_PROG_MAKE_SET

DODS_PROG_PERL
dnl DODS_PROG_GTAR

AC_PATH_PROG([CURL], [curl], [*** Where is curl? ***],
	         [/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/sbin])

AC_CONFIG_FILES([nph-dods dods.rc Makefile], [chmod +x nph-dods])
AC_OUTPUT
