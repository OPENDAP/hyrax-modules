
# m4 macros from the Unidata netcdf 2.3.2 pl4 distribution. Modified for use
# with GNU Autoconf 2.1. I renamed these from UC_* to DODS_* so that there
# will not be confusion when porting future versions of netcdf into the DODS
# source distribution. Unidata, Inc. wrote the text of these macros and holds
# a copyright on them.
#
# jhrg 3/27/95
#
# Added some of my own macros (don't blame Unidata for them!) starting with
# DODS_PROG_LEX and down in the file. jhrg 2/11/96
#
# $Id: acinclude.m4,v 1.13 1996/09/20 22:55:14 jimg Exp $

# Check for fill value usage.

AC_DEFUN(DODS_FILLVALUES, [dnl
    AC_MSG_CHECKING(for fill value usage)
    if test "${OLD_FILLVALUES-}" = yes
    then
        OLD_FILLVALUES=1
	AC_DEFINE(OLD_FILLVALUES)
	AC_MSG_RESULT(using old fill values)
    else
	OLD_FILLVALUES=0
	AC_MSG_RESULT(not using old fill values)
    fi
    AC_SUBST(OLD_FILLVALUES)])

# Check endianness. This program returns true on a little endian machine,
# false otherwise.

ENDIAN_CHECK_PRG="main() {
    long i = 0;
    unsigned char *ip = (unsigned char *)&i;
    *ip	= 0xff;
    /* i == 0xff if little endian or if sizeof(long)==sizeof(char) */
    exit(i == 0xff ? 0: 1);
}"

AC_DEFUN(DODS_SWAP, [dnl
    AC_REQUIRE([AC_CANONICAL_HOST])
    AC_MSG_CHECKING(endianess)
    AC_TRY_RUN([${ENDIAN_CHECK_PRG}], [SWAP=-DSWAP], [SWAP=""], [dnl
	case "$host" in
	    i386* | dec*) SWAP=-DSWAP;;
	    *) SWAP="";;
	esac])
    if test -z "$SWAP"
    then
	AC_MSG_RESULT(big endian)
    else
	AC_MSG_RESULT(little endian)
    fi
    AC_SUBST(SWAP)])

# Check type of 32-bit `network long' integer.

NETLONG_CHECK_PGM="main() {exit(sizeof(long) == 4 ? 0: 1);}"

AC_DEFUN(DODS_NETLONG, [dnl
    AC_REQUIRE([AC_CANONICAL_HOST])
    AC_MSG_CHECKING(net long type)
    AC_TRY_RUN([${NETLONG_CHECK_PGM}], [NETLONG=""], 
	       [NETLONG='-DNETLONG=int'], [dnl
	case "$host" in
            *alpha*) NETLONG='-DNETLONG=int';;
	    *) NETLONG="";;
        esac])
   if test -z "$NETLONG"
   then
	AC_MSG_RESULT(long)
   else
	AC_MSG_RESULT(int)
   fi
   AC_SUBST(NETLONG)])

# Set the value of a variable.  Use the environment if possible; otherwise
# set it to a default value.  Call the substitute routine.

AC_DEFUN(DODS_DEFAULT, [$1=${$1-"$2"}; AC_SUBST([$1])])

# Look for Flex version 2.5.2 or greater. 
# NB: on some machines `flex -V' writes to stderr *not* stdout while `|'
# connects only stdout to stdin. Thus for portability, stderr must be
# connected to stdout manually (This is true for IRIX-5.2).

# NB: had to use [[ for [ due to m4's quoting. 11/17/95.

AC_DEFUN(DODS_PROG_LEX, [dnl
    AC_PROG_LEX
    case "$LEX" in
	flex)
	    flex_ver1=`flex -V 2>&1 | sed 's/[[^0-9]]*\(.*\)/\1/'`
	    flex_ver2=`echo $flex_ver1 | sed 's/\.//g'`
	    if test -z $flex_ver2 -a $flex_ver2 -lt 252
	    then
		AC_MSG_WARN(Flex version: found ${flex_venr1} should be at least 2.5.2)
	    else
		AC_MSG_RESULT(Found flex version ${flex_ver1}.)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(Flex is required for grammar changes.)
	    ;;
    esac])

# Look for Bison version 1.22 or greater

AC_DEFUN(DODS_PROG_BISON, [dnl
    AC_CHECK_PROG(YACC,bison,bison)
    case "$YACC" in
	bison)
	    bison_ver1=`bison -V 2>&1 | sed 's/[[^0-9]]*\(.*\)/\1/'`
	    bison_ver2=`echo $bison_ver1 | sed 's/\.//g'`
	    if test -z $bison_ver2 -a $bison_ver2 -lt 122
	    then
		AC_MSG_WARN(Bison version: found ${bison_ver1} should be at least 1.22)
	    else
		AC_MSG_RESULT(Found bison version ${bison_ver1}.)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(Bison is required for grammar changes.)
	    ;;
    esac])

# Check for support of `-g' by gcc (SGI does not support it unless your using
# gas (and maybe ld).

NULL_PROGRAM="mail() {}"

AC_DEFUN(DODS_CHECK_GCC_DEBUG, [dnl
    AC_MSG_CHECKING(for gcc debugging support)
    msgs=`gcc -g /dev/null 2>&1`
    if echo $msgs | egrep "\`-g' option not supported"
    then		
	CFLAGS=`echo $CFLAGS | sed 's/-g//'`;
	CXXFLAGS=`echo $CXXFLAGS | sed 's/-g//'`;
	LDFLAGS=`echo $LDFLAGS | sed 's/-g//'`;
	AC_MSG_RESULT(not supported)
    else
	AC_MSG_RESULT(supported)
    fi])

dnl look for expect 5.20 *or* 5.19. NB: 5.19 has a bug that DODS exercises
dnl but there are patched version of the library out so many will work even
dnl though the official release won't.

AC_DEFUN(DODS_FIND_EXPECT, [dnl
    HAVE_EXPECT=0

    AC_CHECK_LIB(expect5.20, main, 
	HAVE_EXPECT=1;expect=expect5.20;tcl=tcl7.5, , -ltcl7.5)

    if test $HAVE_EXPECT = "1"
    then
	LIBS="$LIBS -l${expect} -l${tcl}"
    else
	AC_CHECK_LIB(expect, main, \
	    HAVE_EXPECT=1;expect=expect;tcl=tcl7.4, , -ltcl7.4)
	if test $HAVE_EXPECT = "1"
	then
	    LIBS="$LIBS -l${expect} -l${tcl}"
	fi
    fi

    dnl Part two: Once we have found expect (and tcl), locate the tcl include
    dnl directory. Assume that all the tcl includes live where tclRegexp.h
    dnl does.

    AC_CHECK_HEADER(tclRegexp.h, found=1, found=0)

    dnl Look some other places if not in the standard ones.
   
    tcl_include_paths="/usr/local/src/tcl7.4/ /usr/local/src/tcl7.5/generic"

    if test $found -eq 0
    then
        AC_MSG_CHECKING(for tclRegex.h in some more places)

	for d in $tcl_include_paths
	do
	    if test -f ${d}/tclRegexp.h
	    then
		INCS="$INCS -I${d}"
		AC_MSG_RESULT($d)
		found=1
	        break
	    fi
        done
	if test $found -eq 0 
	then
	    AC_MSG_WARN(not found)
	fi
    fi

    AC_DEFINE_UNQUOTED(HAVE_EXPECT, $HAVE_EXPECT)])

AC_DEFUN(DODS_EFENCE, [dnl
    AC_ARG_ENABLE(efence,
		  [  --enable-efence         Runtime memory checks (malloc)],
		  EFENCE=$enableval, EFENCE=no)

    case "$EFENCE" in
    yes)
      AC_MSG_RESULT(Configuring dynamic memory checks on malloc/free calls)
      LIBS="$LIBS -lefence"
      ;;
    *)
      ;;
    esac])

AC_DEFUN(DODS_DBNEW, [dnl
    AC_ARG_ENABLE(dbnew,
	          [  --enable-dbnew          Runtime memory checks (new)],
		  DBNEW=$enableval, DBNEW=no)

    case "$DBNEW" in
    yes)
      AC_MSG_RESULT(Configuring dynamic memory checks on new/delete calls)
      AC_DEFINE(TRACE_NEW)
      LIBS="$LIBS -ldbnew"
      ;;
    *)
      ;;
    esac])

AC_DEFUN(DODS_DEBUG_OPTION, [dnl
    AC_ARG_ENABLE(debug, 
		  [  --enable-debug=ARG      Program instrumentation (1,2)],
		  DEBUG=$enableval, DEBUG=no)

    case "$DEBUG" in
    no) 
      ;;
    1)
      AC_MSG_RESULT(Setting debugging to level 1)
      AC_DEFINE(DODS_DEBUG)
      ;;
    2) 
      AC_MSG_RESULT(Setting debugging to level 2)
      AC_DEFINE(DODS_DEBUG)
      AC_DEFINE(DODS_DEBUG2)
      ;;
    *)
      AC_MSG_ERROR(Bad debug value)
      ;;
    esac])

AC_DEFUN(DODS_FIND_WWW_INCLUDES, [dnl

    AC_MSG_CHECKING(for the WWW library include files)

    dods_www_includes=
    for d in /usr/local/src/WWW /usr/local/WWW ${dods_root}/includes/WWW \
	     `ls -dr /usr/local/src/WWW[1-9]* 2>/dev/null` \
	     `ls -dr /usr/local/WWW[1-9]* 2>/dev/null`
    do
	if test -f ${d}/Library/Implementation/WWWCore.h
	then
	    dods_www_includes=${d}/Library/Implementation
	    break
	fi
    done

    if test "$dods_www_includes"
    then
	INCS="$INCS -I${dods_www_includes}"
	AC_MSG_RESULT($dods_www_includes)
	AC_SUBST(INCS)
    else
	AC_MSG_ERROR(not found!)
    fi])

AC_DEFUN(DODS_WWW_LIBRARY, [dnl

    AC_ARG_WITH(www,
		[  --with-www=ARG          Where is the WWW libarry (directory)],
		WWW_LIB=${withval}, WWW_LIB=)

    if test "$WWW_LIB"
    then
	    LDFLAGS="$LDFLAGS -L${WWW_LIB}"
	    AC_SUBST(LDFLAGS)
    fi])

AC_DEFUN(DODS_SEM, [dnl

    found=0
    AC_CHECK_HEADERS(sys/sem.h, found=1, found=0)
    if test $found -eq 1
    then
        AC_CHECKING(semaphore features in sem.h)
        if grep 'extern int  *semctl.*(' /usr/include/sys/sem.h >/dev/null 2>&1
        then
            AC_DEFINE(HAVE_SEM_PROTO, 1)
        else
            AC_DEFINE(HAVE_SEM_PROTO, 0)
        fi

        if grep 'union semun  *{' /usr/include/sys/sem.h >/dev/null 2>&1
        then
           AC_DEFINE(HAVE_SEM_UNION, 1)
        else
           AC_DEFINE(HAVE_SEM_UNION, 0)
        fi
    fi])

# Find the matlab root directory

AC_DEFUN(DODS_MATLAB, [dnl
    AC_MSG_CHECKING(for matlab root)

    MATLAB_ROOT=`cmex -v 2>&1 | awk '/MATLAB *= / {print}'`
    MATLAB_ROOT=`echo $MATLAB_ROOT | sed 's@[[^/]]*\(/.*\)@\1@'`
    
    AC_SUBST(MATLAB_ROOT)
    AC_MSG_RESULT($MATLAB_ROOT)])

# Find the root directory of the current rev of gcc

AC_DEFUN(DODS_GCC, [dnl
    AC_MSG_CHECKING(for gcc's libgcc.a)

    GCC_ROOT=`gcc -v 2>&1 | awk '/specs/ {print}'`
    GCC_ROOT=`echo $GCC_ROOT | sed 's@[[^/]]*\(/.*\)/specs@\1@'` 
    
    AC_SUBST(GCC_ROOT)
    AC_MSG_RESULT($GCC_ROOT)])


AC_DEFUN(DODS_OS, [dnl
    AC_MSG_CHECKING(type of operating system)
    if test -z "$OS"; then
      OS=`uname -s | tr '[A-Z]' '[a-z]' | sed 's;/;;g'`
      if test -z "$OS"; then
        echo "OS:operating system:sunos5" >> confdefs.missing
      fi
    fi
    case $OS in
        aix)
dnl            OS_NAME=`uname -s`
dnl            OS_MAJOR=`uname -v | sed 's/[^0-9]*\([0-9]*\)\..*/\1/'`
            ;;
        hp-ux)
            OS=hpux`uname -r | sed 's/[A-Z.0]*\([0-9]*\).*/\1/'`
dnl            OS_NAME=HPUX
dnl            OS_MAJOR=`uname -r | sed 's/[A-Z.0]*\([0-9]*\).*/\1/'`
            ;;
        irix)
            OS=${OS}`uname -r | sed 's/\..*//'`
dnl            OS_NAME=IRIX
dnl            OS_MAJOR=`uname -r | sed 's/\..*//'`
            ;;
        osf*)
dnl            OS_NAME=OSF1
dnl            OS_MAJOR=`uname -r | sed 's/[^0-9]*\([0-9]*\)\..*/\1/'`
            ;;
        sn*)
            OS=unicos
dnl            OS_NAME=UNICOS
dnl            OS_MAJOR=`uname -r | sed 's/[^0-9]*\([0-9]*\)\..*/\1/'`
            ;;
        sunos)
dnl            OS_NAME=SunOS
            OS_MAJOR=`uname -r | sed 's/\..*//'`
            OS=$OS$OS_MAJOR
            ;;
        ultrix)
            case `uname -m` in
            VAX)
                OS=vax-ultrix
                ;;
            esac
dnl           OS_NAME=ULTRIX
dnl            OS_MAJOR=`uname -r | sed 's/\..*//'`
            ;;
        *)
            # On at least one UNICOS system, 'uname -s' returned the
            # hostname (sigh).
            if uname -a | grep CRAY >/dev/null; then
                OS=unicos
dnl               OS_NAME=UNICOS
dnl            else
dnl               OS_NAME=`uname -s | sed 's/[^A-Za-z0-9_]//g'`
            fi
dnl            OS_MAJOR=`uname -r | sed 's/[^0-9]*\([0-9]*\)\..*/\1/'`
            ;;
    esac

    # Adjust OS for CRAY MPP environment.
    #
    case "$OS" in
    unicos)

        case "$CC$TARGET$CFLAGS" in
        *cray-t3*)
            OS=unicos-mpp
            ;;
        esac
        ;;
    esac

    AC_SUBST(OS)
dnl    AC_DEFINE(OS_NAME, $OS_NAME)
dnl    AC_DEFINE(OS_MAJOR, $OS_MAJOR)

    AC_MSG_RESULT($OS)])


AC_DEFUN(DODS_MACHINE, [dnl
    AC_MSG_CHECKING(type of machine)

    if test -z "$MACHINE"; then
    MACHINE=`uname -m | tr [A-Z] [a-z]`
    case $OS in
        aix*)
            MACHINE=rs6000
            ;;
        hp*)
            MACHINE=hp`echo $MACHINE | sed 's%/.*%%'`
            ;;
        sunos*)
            case $MACHINE in
                sun4*)
                    MACHINE=sun4
                    ;;
            esac
            ;;
        irix*)
            case $MACHINE in
                ip20)
                    MACHINE=sgi
                    ;;
            esac
            ;;
    esac

    if test -z "$MACHINE"; then
      echo "MACHINE:machine hardware type:sun4" >> confdefs.missing
    fi
    fi

    AC_SUBST(MACHINE)
    AC_MSG_RESULT($MACHINE)])

