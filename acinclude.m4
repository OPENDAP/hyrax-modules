
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
# $Id: acinclude.m4,v 1.24 1996/11/13 23:39:05 jimg Exp $

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

# Look for Bison version 1.22 or greater. Define DODS_BISON_VER to be the
# version number without the decimal point.

AC_DEFUN(DODS_PROG_BISON, [dnl
    AC_CHECK_PROG(YACC,bison,bison)
    case "$YACC" in
	bison)
	    bison_ver1=`bison -V 2>&1 | sed 's/[[^0-9]]*\(.*\)/\1/'`
	    bison_ver2=`echo $bison_ver1 | sed 's/\.//g'`
	    AC_DEFINE_UNQUOTED(DODS_BISON_VER, $bison_ver2)
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

dnl look for expect 5.21, 5.20 *or* 5.19. NB: 5.19 has a bug that DODS
dnl exercises but there are patched version of the library out so many will
dnl work even though the official release won't.

AC_DEFUN(DODS_FIND_EXPECT, [dnl
    HAVE_EXPECT=0

    AC_CHECK_LIB(expect521, main, HAVE_EXPECT=1;expect=expect521;tcl=tcl76;\
	LIBS="$LIBS -l${expect} -l${tcl}", , -ltcl76)

    if test $HAVE_EXPECT -eq 0; then
        AC_CHECK_LIB(expect5.21, main, \
		HAVE_EXPECT=1;expect=expect5.21;tcl=tcl7.6;\
		LIBS="$LIBS -l${expect} -l${tcl}", , -ltcl7.6)
    fi

    if test $HAVE_EXPECT -eq 0; then
        AC_CHECK_LIB(expect520, main, \
		HAVE_EXPECT=1;expect=expect520;tcl=tcl75;\
		LIBS="$LIBS -l${expect} -l${tcl}", , -ltcl75)
    fi

    if test $HAVE_EXPECT -eq 0; then
        AC_CHECK_LIB(expect5.20, main, \
		HAVE_EXPECT=1;expect=expect5.20;tcl=tcl7.5;\
		LIBS="$LIBS -l${expect} -l${tcl}", , -ltcl7.5)
    fi

    if test $HAVE_EXPECT -eq 0; then
        AC_CHECK_LIB(expect, main, \
		HAVE_EXPECT=1;expect=expect;tcl=tcl7.4;\
		LIBS="$LIBS -l${expect} -l${tcl}", , -ltcl7.4)
    fi

    dnl Part two: Once we have found expect (and tcl), locate the tcl include
    dnl directory. Assume that all the tcl includes live where tclRegexp.h
    dnl does.

    AC_CHECK_HEADER(tclRegexp.h, found=1, found=0)

    dnl Look some other places if not in the standard ones.
   
    tcl_include_paths="$DODS_ROOT/third_party/tcl7.6/generic \
			/usr/local/src/tcl7.5/generic /usr/local/src/tcl7.4/"

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

dnl depricated

AC_DEFUN(DODS_FIND_WWW_ROOT, [dnl

    AC_MSG_CHECKING(for the WWW library root directory)

    WWW_ROOT=

    for p in /usr/local/src/WWW /usr/local/WWW \
	     /usr/local/src/w3c-libwww /usr/local/w3c-libwww \
	     /usr/contrib/src/w3c-libwww /usr/contrib/w3c-libwww \
	     $DODS_ROOT/third-party/w3c-libwww
    do
        if test "$WWW_ROOT"
	then
	    break
	fi
	dnl `ls -dr' lists dirs without descending and reverses ordering
	for d in `ls -dr ${p}[[-.0-9]]* 2>/dev/null`
	do
	    if test -f ${d}/Library/src/WWWCore.h
	    then
	        WWW_ROOT=${d}
	        break
	    fi
	done
    done

    if test "$WWW_ROOT"
    then
	AC_MSG_RESULT($WWW_ROOT)
	AC_SUBST(WWW_ROOT)
	INCS="$INCS -I$(WWW_ROOT) -I$(WWW_ROOT)/Library/src"
	AC_SUBST(INCS)
    else
	AC_MSG_WARN(not found!)
    fi])

AC_DEFUN(DODS_WWW_ROOT, [dnl

    AC_ARG_WITH(www,
	[  --with-www=DIR          Directory containing the W3C software],
	WWW_ROOT=${withval}, WWW_ROOT=)

    if test "$WWW_ROOT"
    then
	AC_SUBST(WWW_ROOT)
	INCS="$INCS -I$(WWW_ROOT) -I$(WWW_ROOT)/Library/src"
	AC_SUBST(INCS)
	LIBS="$LIBS -L$(WWW_ROOT)/Library/src"
	AC_SUBST(LIBS)
	AC_MSG_RESULT(Set WWW root directory to $WWW_ROOT) 
    else
	DODS_FIND_WWW_ROOT
    fi])

AC_DEFUN(DODS_SEM, [dnl

    found=0
    AC_CHECK_HEADERS(sys/sem.h, found=1, found=0)
    if test $found -eq 1
    then
        AC_CHECKING(semaphore features in sem.h)
        if grep 'int *semctl.*(' /usr/include/sys/sem.h >/dev/null 2>&1
        then
            AC_DEFINE(HAVE_SEM_PROTO, 1)
        else
            AC_DEFINE(HAVE_SEM_PROTO, 0)
        fi

        if grep 'union *semun *{' /usr/include/sys/sem.h >/dev/null 2>&1
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
    
    dnl Find the lib directory (which is named according to machine type).
    matlab_lib_dir=`find $MATLAB_ROOT -name libmat.a \
		| sed 's@\(.*\)/libmat.a@\1@'`
    LDFLAGS="$LDFLAGS -L$matlab_lib_dir"

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
        AC_MSG_WARN(OS unknown!)
      fi
    fi
    case $OS in
        aix)
            ;;
        hp-ux)
            OS=hpux`uname -r | sed 's/[A-Z.0]*\([0-9]*\).*/\1/'`
            ;;
        irix)
            OS=${OS}`uname -r | sed 's/\..*//'`
            ;;
        osf*)
            ;;
        sn*)
            OS=unicos
            ;;
        sunos)
            OS_MAJOR=`uname -r | sed 's/\..*//'`
            OS=$OS$OS_MAJOR
            ;;
        ultrix)
            case `uname -m` in
            VAX)
                OS=vax-ultrix
                ;;
            esac
            ;;
        *)
            # On at least one UNICOS system, 'uname -s' returned the
            # hostname (sigh).
            if uname -a | grep CRAY >/dev/null; then
                OS=unicos
            fi
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

    AC_MSG_RESULT($OS)])


AC_DEFUN(DODS_MACHINE, [dnl
    AC_MSG_CHECKING(type of machine)

    if test -z "$MACHINE"; then
    MACHINE=`uname -m | tr '[A-Z]' '[a-z]'`
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

dnl Check for exceptions handling support. From Todd.

# Check for exceptions handling support
AC_DEFUN(DODS_CHECK_EXCEPTIONS, [dnl
    AC_LANG_CPLUSPLUS
    AC_MSG_CHECKING("for exception handling support in C++ compiler")
    OLDCXXFLAGS="$CXXFLAGS"
    if test $CXX = "g++"; then
       CXXFLAGS="$OLDCXXFLAGS -fhandle-exceptions -O0"
    fi
    EXCEPTION_CHECK_PRG="int foo(void) {
                              throw int();
                         }
                         main() {
                              try { foo(); }
                              catch(int) { exit(0); }
                              exit(1);
                         }"

    AC_TRY_RUN([${EXCEPTION_CHECK_PRG}],
        AC_MSG_RESULT(yes),
	[dnl
        AC_MSG_RESULT(no)
        AC_MSG_WARN("Compiling without exception handling.  See README.")
        CXXFLAGS=$OLDCXXFLAGS
        DEFS="$DEFS -DNO_EXCEPTIONS"
        ],true)])

#check for hdf libraries
AC_DEFUN(DODS_HDF_LIBRARY, [dnl
    AC_ARG_WITH(hdf,
        [  --with-hdf=ARG          Where is the HDF libarry (directory)],
        HDF_PATH=${withval}, HDF_PATH=/usr/local/hdf)
    if test "$HDF_PATH"
    then
            LDFLAGS="$LDFLAGS -L${HDF_PATH}/lib"
            AC_SUBST(LDFLAGS)
            INCS="$INCS -I${HDF_PATH}/include"
            AC_SUBST(INCS)
    fi
    AC_CHECK_LIB(z, inflate_flush, LIBS="-lz $LIBS", nohdf=1)
    AC_CHECK_LIB(jpeg, jpeg_start_compress, LIBS="-ljpeg $LIBS", nohdf=1)
    AC_CHECK_LIB(df, Hopen, LIBS="-ldf $LIBS" , nohdf=1)
    AC_CHECK_LIB(mfhdf, SDstart, LIBS="-lmfhdf $LIBS" , nohdf=1)])

AC_DEFUN(DODS_FIND_DSP_ROOT, [dnl

    AC_MSG_CHECKING(for the DSP library root directory)

    DSP_ROOT=

    for p in /usr/local/src/DSP /usr/local/DSP \
	     /usr/local/src/dsp /usr/local/dsp \
	     /usr/contrib/src/dsp /usr/contrib/dsp \
	     $DODS_ROOT/third-party/dsp /usr/dsp
    do
        if test "$DSP_ROOT"; then
	    break
	fi
	dnl `ls -dr' lists dirs without descending and reverses ordering
	for d in `ls -dr ${p}[[-.0-9]]* 2>/dev/null`
	do
	    if test -f ${d}/inc/dsplib.h; then
	        DSP_ROOT=${d}
	        break
	    fi
	done
    done

    if test "$DSP_ROOT"
    then
	AC_MSG_RESULT($DSP_ROOT)
	AC_SUBST(DSP_ROOT)
	INCS="$INCS -I$(DSP_ROOT)/inc"
	AC_SUBST(INCS)
	LIBS="$LIBS -L$(DSP_ROOT)/lib -L$(DSP_ROOT)/shlib"
	AC_SUBST(LIBS)
    else
	AC_MSG_WARN(not found!)
    fi])

AC_DEFUN(DODS_DSP_ROOT, [dnl

    AC_ARG_WITH(dsp,
		[  --with-dsp=DIR          Directory containing DSP software from U of Miami],
		DSP_ROOT=${withval}, DSP_ROOT=)

    if test "$DSP_ROOT"
    then
	AC_SUBST(DSP_ROOT)
	INCS="$INCS -I$(DSP_ROOT)/inc"
	AC_SUBST(INCS)
	LIBS="$LIBS -L$(DSP_ROOT)/lib -L$(DSP_ROOT)/shlib"
	AC_SUBST(LIBS)
	AC_MSG_RESULT(Set DSP root directory to $DSP_ROOT) 
    else
	DODS_FIND_DSP_ROOT
    fi])

AC_DEFUN(DODS_CHECK_SIZES, [dnl
    dnl Ignore the errors about AC_TRY_RUN missing an argument. jhrg 5/2/95

    AC_C_CROSS

    if test "$cross_compiling" = yes
    then
	    case "$host" in
	    *alpha*) AC_DEFINE(SIZEOF_CHAR, 1)
		     AC_DEFINE(SIZEOF_DOUBLE, 8)
		     AC_DEFINE(SIZEOF_FLOAT, 4)
		     AC_DEFINE(SIZEOF_INT, 4)
		     AC_DEFINE(SIZEOF_LONG, 8)
		     ;;
	    *)	AC_MSG_WARN(Assuming that your target is a 32bit machine)
		    AC_DEFINE(SIZEOF_CHAR, 1)
		    AC_DEFINE(SIZEOF_DOUBLE, 8)
		    AC_DEFINE(SIZEOF_FLOAT, 4)
		    AC_DEFINE(SIZEOF_INT, 4)
		    AC_DEFINE(SIZEOF_LONG, 4)
		    ;;
	    esac
    else
	    AC_CHECK_SIZEOF(int)
	    AC_CHECK_SIZEOF(long)
	    AC_CHECK_SIZEOF(char)
	    AC_CHECK_SIZEOF(double)
	    AC_CHECK_SIZEOF(float)

    if test $ac_cv_sizeof_long -eq 4 
    then
	ARCHFLAG=ARCH_32BIT
	AC_SUBST(ARCHFLAG)
	DEFS="-DARCH_32BIT $DEFS"
    elif test $ac_cv_sizeof_long -eq 8
    then
	ARCHFLAG=ARCH_64BIT
	AC_SUBST(ARCHFLAG)
	DEFS="-DARCH_64BIT $DEFS"
    else
	AC_MSG_ERROR(Could not determine architecture size - 32 or 64 bits)
    fi

    fi])

dnl Check for -lsocket and -lnsl for sockets and xdr code. Needed for
dnl Solaris. 

AC_DEFUN(DODS_LIBS, [dnl
    AC_CHECK_LIB(socket, bind)
    AC_CHECK_LIB(nsl, xdr_int)])

