
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
# I've added a table of contents for this file. jhrg 2/2/98
# 
# 1. Unidata-derived macros. 
# 2. Macros for finding libraries used by the core software.
# 3. Macros used to test things about the compiler
# 4. Macros for locating various systems (Matlab, etc.)
# 5. Macros used to test things about the computer/OS/hardware
#
# $Id: acinclude.m4,v 1.70 2000/10/30 17:17:48 jimg Exp $

# 1. Unidata's macros
#-------------------------------------------------------------------------

builtin(include, ud_aclocal.m4)

# 2. Finding libraries 
#
# To use these in DODS software, in the Makefile.in use LIBS XTRALIBS for 
# non-gui and LIBS GUILIBS XTRALIBS for the clients that link with the 
# gui DAP++ library. These should be set by line like LIBS=@LIBS@, etc.
# Then the group should be used on the link line. 10/16/2000 jhrg
#--------------------------------------------------------------------------

AC_DEFUN(DODS_PACKAGES_SUPPORT, [dnl
    # Where does DODS live?
    AC_REQUIRE([DODS_GET_DODS_ROOT])
    # Find a good C compiler (hopefully gcc).
    AC_REQUIRE([AC_PROG_CC])
    # Find out about -lns and -lsocket
    # I removed the following line because DODS_LIBS includes libraries that 
    # TK also uses. Since We can get those libraries from the TK Script
    # (which should be run in configure) there's no need to look for the
    # libraries here, too. 1/10/2000 jhrg
    # AC_REQUIRE([DODS_LIBS])
    # Find the full name of the packages directory
    AC_REQUIRE([DODS_FIND_PACKAGES_DIR])
    # Assume that we always search the packages/lib directory for libraries.
    # LDFLAGS="$LDFLAGS -L$DODS_PACKAGES_DIR/lib"
    # Assume that we can always search packages/include directory for include 
    # files. 
    # INCS="$INCS -I$DODS_PACKAGES_DIR/include"
    # Initialize $packages to null.
    packages=""
    AC_SUBST(packages)])

# This macro does things that the other library-finding macros rely on. 
# It must be run before the other library macros. jhrg 2/2/98
#
# Define the C preprocessor symbol `DODS_ROOT' to be the full path to the top
# of the DODS tree (e.g., /usr/local/DODS-2.15). Also substitute that string
# for @dods_root@ and @dir@ and set it to the shell variable $dods_root. Thus
# Makefile.in files can use the path as can configure.in files.

AC_DEFUN(DODS_GET_DODS_ROOT, [dnl
    AC_MSG_CHECKING([for the DODS root pathanme])
    fullpath=`pwd`
    dir=`basename ${fullpath}`
    dods_root=`echo $fullpath | sed 's@\(.*DODS[[-.0-9a-z]]*\).*@\1@'`
    AC_MSG_RESULT($dods_root)
    AC_DEFINE_UNQUOTED(DODS_ROOT, "$dods_root")
    AC_SUBST(dir)
    AC_SUBST(fullpath)
    AC_SUBST(dods_root)])

AC_DEFUN(DODS_FIND_PACKAGES_DIR, [dnl
    AC_MSG_CHECKING("for the packages directory")
    # Where does DODS live?
    AC_REQUIRE([DODS_GET_DODS_ROOT])
    DODS_PACKAGES_DIR=`ls -1d $dods_root/src/packages* 2> /dev/null`
    if test -z "$DODS_PACKAGES_DIR"
    then
	AC_MSG_ERROR("Could not find the third-party packages!")
    fi
    AC_MSG_RESULT($DODS_PACKAGES_DIR)
    AC_SUBST(DODS_PACKAGES_DIR)])

# Because the www library is now included in the DODS_ROOT/packages-*/ 
# directory, look there for the include files. Users can specify a 
# different directory using --with-www. jhrg 2/4/98
#
# This used to be DODS_WWW_ROOT, but it is no longer called directly in
# configure.in files. Instead use DODS_WWW_LIB. jhrg 2/3/98

AC_DEFUN(DODS_FIND_WWW_ROOT, [dnl
    AC_MSG_CHECKING([for the libwww root])
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])

    AC_ARG_WITH(www,
	[  --with-www=DIR          Directory containing the W3C header files],
	WWW_ROOT=${withval}, WWW_ROOT=$DODS_ROOT/include/w3c-libwww)
dnl	WWW_ROOT=${withval}, WWW_ROOT=$DODS_PACKAGES_DIR/include/w3c-libwww)

    AC_SUBST(WWW_ROOT)
    INCS="$INCS -I\$(WWW_ROOT)"
    AC_SUBST(INCS)
    AC_MSG_RESULT($WWW_ROOT)])

AC_DEFUN(DODS_COMPRESSION_LIB, [dnl
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])
    AC_CHECK_LIB(z, zlibVersion,
		 HAVE_Z=1; LIBS="$LIBS -lz",
		 packages="$packages libz"; HAVE_Z=1; LIBS="$LIBS -lz")
    AC_SUBST(packages)])

AC_DEFUN(DODS_RX_LIB, [dnl
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])
    AC_CHECK_LIB(rx, rx_version_string,
		 HAVE_RX=1; LIBS="$LIBS -lrx",
		 packages="$packages libz"; HAVE_RX=1; LIBS="$LIBS -lrx")
    AC_SUBST(packages)])

# Look for the web library.
AC_DEFUN(DODS_WWW_LIB, [dnl
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])
    AC_REQUIRE([DODS_FIND_WWW_ROOT])
    HAVE_WWW=1; LIBS="-lwww $LIBS"
    AC_DEFINE_UNQUOTED(HAVE_WWW, $HAVE_WWW)])

# Check for the Tcl and Tk libraries. These are required. 8/3/99 jhrg
# These are required for the progress indicator, only. 3/17/2000 jhrg

AC_DEFUN(DODS_TCL_LIB, [dnl
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])
    GUILIBS="$GUILIBS -ltcl8.3"
    AC_DEFINE_UNQUOTED(HAVE_TCL, $HAVE_TCL)])
     
AC_DEFUN(DODS_TK_LIB, [dnl
    AC_REQUIRE([DODS_PACKAGES_SUPPORT])
    GUILIBS="$GUILIBS -ltk8.3"
    AC_DEFINE_UNQUOTED(HAVE_TK, $HAVE_TK)])

AC_DEFUN(DODS_GUILIBS, [dnl
    AC_REQUIRE([DODS_FIND_PACKAGES_DIR])
    AC_REQUIRE([DODS_TK_LIB])
    AC_REQUIRE([DODS_TCL_LIB])

    . ${DODS_ROOT}/lib/tkConfig.sh
dnl    . ${DODS_PACKAGES_DIR}/lib/tkConfig.sh
    GUILIBS="$GUILIBS $TK_LIBS"
    AC_SUBST(GUILIBS)])

# Check for the existence of the -lsocket and -lnsl libraries.
# The order here is important, so that they end up in the right
# order in the command line generated by make.  Here are some
# special considerations:
# 1. Use "connect" and "accept" to check for -lsocket, and
#    "gethostbyname" to check for -lnsl.
# 2. Use each function name only once:  can't redo a check because
#    autoconf caches the results of the last check and won't redo it.
# 3. Use -lnsl and -lsocket only if they supply procedures that
#    aren't already present in the normal libraries.  This is because
#    IRIX 5.2 has libraries, but they aren't needed and they're
#    bogus:  they goof up name resolution if used.
# 4. On some SVR4 systems, can't use -lsocket without -lnsl too.
#    To get around this problem, check for both libraries together
#    if -lsocket doesn't work by itself.
#
# From Tcl7.6 configure.in. jhrg 11/18/96

AC_DEFUN(DODS_XTRALIBS, [dnl
    checkBoth=0
    AC_CHECK_FUNC(connect, checkSocket=0, checkSocket=1)
    if test "$checkSocket" = 1; then
	AC_CHECK_LIB(socket, main, XTRALIBS="$XTRALIBS -lsocket", checkBoth=1)
    fi
    if test "$checkBoth" = 1; then
	oldLibs=$XTRALIBS
	XTRALIBS="$XTRALIBS -lsocket -lnsl"
	AC_CHECK_FUNC(accept, checkNsl=0, [XTRALIBS=$oldLibs])
    fi
    AC_CHECK_FUNC(gethostbyname, , AC_CHECK_LIB(nsl, main, 
		  [XTRALIBS="$XTRALIBS -lnsl"]))
    AC_SUBST(XTRALIBS)])

# Electric fence and dbnew are used to debug malloc/new and free/delete.
# I assume that if you use these switches you know enough to build the 
# libraries. 2/3/98 jhrg

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

# check for hdf libraries
# cross-compile problem with test option -d
AC_DEFUN(DODS_HDF_LIBRARY, [dnl
    AC_ARG_WITH(hdf,
        [  --with-hdf=ARG          Where is the HDF library (directory)],
        HDF_PATH=${withval}, HDF_PATH="$HDF_PATH")
    if test ! -d "$HDF_PATH"
    then
        HDF_PATH="/usr/local/hdf"
    fi
    if test "$HDF_PATH"
    then
            LDFLAGS="$LDFLAGS -L${HDF_PATH}/lib"
            INCS="$INCS -I${HDF_PATH}/include"
            AC_SUBST(INCS)
    fi

dnl None of this works with HDF 4.1 r1. jhrg 8/2/97

    AC_CHECK_LIB(z, deflate, LIBS="-lz $LIBS", nohdf=1)
    AC_CHECK_LIB(jpeg, jpeg_start_compress, LIBS="-ljpeg $LIBS", nohdf=1)
    AC_CHECK_LIB(df, Hopen, LIBS="-ldf $LIBS" , nohdf=1)
    AC_CHECK_LIB(mfhdf, SDstart, LIBS="-lmfhdf $LIBS" , nohdf=1)])

# 3. Compiler test macros
#--------------------------------------------------------------------------

# Look for Flex version 2.5.2 or greater. 
# NB: on some machines `flex -V' writes to stderr *not* stdout while `|'
# connects only stdout to stdin. Thus for portability, stderr must be
# connected to stdout manually (This is true for IRIX-5.2).

# NB: had to use [[ for [ due to m4's quoting. 11/17/95.

AC_DEFUN(DODS_PROG_LEX, [dnl
    AC_PROG_LEX
    case "$LEX" in
	*flex)
	    flex_ver1=`$LEX -V 2>&1 | sed 's/[[^0-9]]*\(.*\)/\1/'`
	    flex_ver2=`echo $flex_ver1 | sed 's/\.//g'`
	    if test -n "$flex_ver2" && test $flex_ver2 -ge 252
	    then
		AC_MSG_RESULT(Found flex version ${flex_ver1}.)
	    else
		AC_MSG_WARN(Flex version: found ${flex_venr1} should be at least 2.5.2)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(Flex is required for grammar changes.)
	    ;;
    esac])

# Look for Bison version 1.24 or greater. Define DODS_BISON_VER to be the
# version number without the decimal point.

AC_DEFUN(DODS_PROG_BISON, [dnl
    AC_CHECK_PROG(YACC,bison,bison)
    case "$YACC" in
	*bison)
	    bison_ver1=`$YACC -V 2>&1 | sed 's/[[^0-9]]*\(.*\)/\1/'`
	    bison_ver2=`echo $bison_ver1 | sed 's/\.//g'`
	    AC_DEFINE_UNQUOTED(DODS_BISON_VER, $bison_ver2)
	    if test -n "$bison_ver2" && test $bison_ver2 -ge 125
	    then
		AC_MSG_RESULT(Found bison version ${bison_ver1}.)
	    else
		AC_MSG_WARN(Bison version: found ${bison_ver1} should be at least 1.25)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(Bison is required for grammar changes.)
	    ;;
    esac])

# Check for support of `-gstabs' by gcc (SGI does not support it unless your 
# using gas (and maybe ld).

NULL_PROGRAM="mail() {}"

# Find the root directory of the current rev of gcc

AC_DEFUN(DODS_GCC, [dnl
    AC_REQUIRE([DODS_GCC_VERSION])
    AC_MSG_CHECKING(for gcc's libgcc.a)

    GCC_ROOT=`gcc -v 2>&1 | awk '/specs/ {print}'`
    GCC_ROOT=`echo $GCC_ROOT | sed 's@[[^/]]*\(/.*\)/specs@\1@'` 
    
    AC_SUBST(GCC_ROOT)
    AC_MSG_RESULT($GCC_ROOT)])

AC_DEFUN(DODS_GCC_VERSION, [dnl
    AC_MSG_CHECKING(for gcc/g++ 2.8 or greater)

    GCC_VER=`gcc -v 2>&1 | awk '/version/ {print}'`
    dnl We need the gcc version number as a number, without `.'s and limited
    dnl to three digits
    GCC_VER=`echo $GCC_VER | sed 's@[[a-z ]]*\([[0-9.]]\)@\1@'`

    case $GCC_VER in
        *egcs*) AC_MSG_RESULT(Found egcs version ${GCC_VER}.) ;;
	2.[[7-9]]*)   AC_MSG_RESULT(Found gcc/g++ version ${GCC_VER}) ;;
        *)      AC_MSG_ERROR(must be at least version 2.7.x) ;;
    esac])

# 4. Macros to locate various programs/systems used by parts of DODS
#---------------------------------------------------------------------------

# Added by Ethan, 1999/06/21
# Look for perl.
# 
# I modified the regexp below to remove any text that follows the version
# number. This extra text was hosing the test. 7/15/99 jhrg

AC_DEFUN(DODS_PROG_PERL, [dnl
    AC_CHECK_PROG(PERL, perl, `which perl`)
    case "$PERL" in
	*perl*)
	    perl_ver=`$PERL -v 2>&1 | awk '/This is perl/ {print}'`
	    perl_ver=`echo $perl_ver | sed 's/This is perl,[[^0-9]]*\([[0-9._]]*\).*/\1/'`
            perl_ver_main=`echo $perl_ver | sed 's/\([[0-9]]*\).*/\1/'`
	    if test -n "$perl_ver" && test $perl_ver_main -ge 5
	    then
		AC_MSG_RESULT(Found perl version ${perl_ver}.)
	    else
		AC_MSG_ERROR(perl version: found ${perl_ver} should be at least 5.000.)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(perl is required.)
	    ;;
    esac

    AC_SUBST(PERL)])

# Added by Ethan, 1999/06/21
# Look for GNU tar.
# 
# I modified the regexp below but it still does not work exactly correctly; 
# the variable tar_ver should have only the version number in it. However,
# my version (1.12) spits out a multi-line thing. The regexp below gets the
# version number from the first line but does not remove the subsequent lines
# of garbage. 7/15/99 jhrg
# Added awk line to handle multiline output. 1999/07/22 erd

AC_DEFUN(DODS_PROG_GTAR, [dnl
    AC_CHECK_PROGS(TAR,gtar tar,tar)
    case "$TAR" in
	*tar)
	    tar_ver=`$TAR --version 2>&1 | awk '/G[[Nn]][[Uu]] tar/ {print}'`
	    tar_ver=`echo $tar_ver | sed 's/.*GNU tar[[^0-9.]]*\([[0-9._]]*\)/\1/'`
	    if test -n "$tar_ver"
	    then
		AC_MSG_RESULT(Found Gnu tar version ${tar_ver}.)
	    else
		AC_MSG_WARN(GNU tar is required for some Makefile targets.)
	    fi
	    ;;
	*)
	    AC_MSG_WARN(GNU tar is required for some Makefile targets.)
	    ;;
    esac

    AC_SUBST(TAR)])

# Find the matlab root directory
# cross-compile problem with test option -d

AC_DEFUN(DODS_MATLAB, [dnl
    AC_ARG_WITH(matlab,
        [  --with-matlab=ARG       Where is the Matlab root directory],
        MATLAB_ROOT=${withval}, MATLAB_ROOT="$MATLAB_ROOT")
    if test "$MATLAB_ROOT" = no; then
        MATLAB_ROOT="$MATLAB_ROOT"
    elif test ! -d "$MATLAB_ROOT"
    then
        MATLAB_ROOT=""
    fi
    if test -z "$MATLAB_ROOT"
    then
        AC_MSG_CHECKING(for matlab root)

	MATLAB_ROOT=`mex -v 2>&1 | awk '/MATLAB *= / {print}'`
	MATLAB_ROOT=`echo $MATLAB_ROOT | sed 's@[[^/]]*\(/.*\)@\1@'`

	if test -z "$MATLAB_ROOT"
	then
	    AC_MSG_ERROR(Matlab not found! Run configure using -with-matlab option)
        else
	    AC_SUBST(MATLAB_ROOT)
	    AC_MSG_RESULT($MATLAB_ROOT)
        fi
    else
        AC_SUBST(MATLAB_ROOT)
        AC_MSG_RESULT("Set Matlab root to $MATLAB_ROOT")
    fi

    dnl Find the lib directory (which is named according to machine type).
    matlab_lib_dir=`find ${MATLAB_ROOT}/extern/lib -name 'libmat*' -print \
		    | sed 's@\(.*\)/libmat.*@\1@'`
    if test "$matlab_lib_dir"
    then
	LDFLAGS="$LDFLAGS -L$matlab_lib_dir"
	dnl This is used by the nph script to set LD_LIBRARY_PATH
	AC_SUBST(matlab_lib_dir)
    fi
    
    dnl sleazy test for version 5; look for the version 4 compat flag

    if grep V4_COMPAT ${MATLAB_ROOT}/extern/include/mat.h > /dev/null 2>&1
    then
       MAT_VERSION_FLAG="-V4"
       MATLIBS="-lmat -lmi -lmx -lut"
    else
       MAT_VERSION_FLAG=""
       MATLIBS="-lmat"
    fi

    AC_CHECK_LIB(ots, _OtsDivide64Unsigned, MATLIBS="$MATLIBS -lots", )
    AC_SUBST(MATLIBS)
    AC_SUBST(MAT_VERSION_FLAG)])

# cross-compile problem with test option -d
AC_DEFUN(DODS_DSP_ROOT, [dnl

    AC_ARG_WITH(dsp,
		[  --with-dsp=DIR          Directory containing DSP software from U of Miami],
		DSP_ROOT=${withval}, DSP_ROOT="$DSP_ROOT")

    if test ! -d "$DSP_ROOT"
    then
        DSP_ROOT=""
    fi
    if test -z "$DSP_ROOT"
    then
	AC_MSG_CHECKING(for the DSP library root directory)

	for p in /usr/local/src/DSP /usr/local/DSP \
		 /usr/local/src/dsp /usr/local/dsp \
		 /usr/contrib/src/dsp /usr/contrib/dsp \
		 $DODS_ROOT/third-party/dsp /usr/dsp /data1/dsp
	do
	    if test -z "$DSP_ROOT"
	    then
	    	for d in `ls -dr ${p}* 2>/dev/null`
		do
		    if test -f ${d}/inc/dsplib.h
		    then
		        DSP_ROOT=${d}
		        break
		    fi
	        done
	    fi
	done
    fi

    if test "$DSP_ROOT"
    then
	AC_SUBST(DSP_ROOT)
	dnl Only add this path to gcc's options... jhrg 11/15/96
	CFLAGS="$CFLAGS -I\$(DSP_ROOT)/inc"
	LDFLAGS="$LDFLAGS -L\$(DSP_ROOT)/lib -L\$(DSP_ROOT)/shlib"
	AC_MSG_RESULT(Set DSP root directory to $DSP_ROOT) 
    else
        AC_MSG_WARN(not found!)
    fi])

# Find IDL. 9/23/99 jhrg

AC_DEFUN(DODS_IDL, [dnl
    AC_ARG_WITH(idl,
        [  --with-idl=ARG       Where is the IDL root directory],
        IDL_ROOT=${withval}, IDL_ROOT="$IDL_ROOT")
    AC_REQUIRE([AC_CANONICAL_HOST])

    AC_MSG_CHECKING(for the IDL root directory)
    if test -z "$IDL_ROOT"
    then
        # Find IDL's root directory by looking at the exectuable and then 
        # finding where that symbolic link points.
        # !!! Doesn't work if idl isn't a symbolic link - erd !!!
        idl_loc=`which idl`
        idl_loc=`ls -l $idl_loc | sed 's/.*->[ ]*\(.*\)$/\1/'`
        IDL_ROOT=`echo $idl_loc | sed 's/\(.*\)\/bin.*/\1/'`
    fi

    AC_MSG_RESULT($IDL_ROOT)
    AC_SUBST(IDL_ROOT)

    # Now find where the IDL 5.2 or later sharable libraries live.
    # NB: This won't work if libraries for several architecutures are 
    # installed for several machines.
    AC_MSG_CHECKING(for the IDL sharable library directory)
    # cd to the IDL root because it is likely a symbolic link and find 
    # won't normally follow symbolic links.
    IDL_LIBS=`(cd $IDL_ROOT; find . -name 'libidl.so' -print)`
    # Strip off the leading `.' (it's there because we ran find in the CWD) 
    # and the name of the library used to find the directory.
    IDL_LIBS=`echo $IDL_LIBS | sed 's/\.\(.*\)\/libidl.so/\1/'`
    IDL_LIBS=${IDL_ROOT}${IDL_LIBS}
    AC_MSG_RESULT($IDL_LIBS)
    AC_SUBST(IDL_LIBS)])


# 5. Misc stuff
#---------------------------------------------------------------------------

# Use the version.h file in the current directory to set the version 
# Makefile varible. All Makefiles should have targets that use this variable
# to rename the directory, build source distribution tarfiles, etc.
AC_DEFUN(DODS_DIRECTORY_VERSION, [dnl
    AC_MSG_CHECKING([version])
    VERSION=`cat version.h`
    AC_MSG_RESULT($VERSION)
    AC_SUBST(VERSION)])

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

AC_DEFUN(DODS_OS, [dnl
    AC_MSG_CHECKING(type of operating system)
    # I have removed the following test because some systems (e.g., SGI)
    # define OS in a way that breaks this code but that is close enough
    # to also be hard to detect. jhrg 3/23/97
    #  if test -z "$OS"; then
    #  fi 
    OS=`uname -s | tr '[[A-Z]]' '[[a-z]]' | sed 's;/;;g'`
    if test -z "$OS"; then
        AC_MSG_WARN(OS unknown!)
    fi
    case $OS in
        aix)
            ;;
        hp-ux)
            OS=hpux`uname -r | sed 's/[[A-Z.0]]*\([[0-9]]*\).*/\1/'`
            ;;
        irix)
            OS=${OS}`uname -r | sed 's/\..*//'`
            ;;
        # I added the following case because the `tr' command above *seems* 
	# to fail on Irix 5. I can get it to run just fine from the shell, 
	# but not in the configure script built using this macro. jhrg 8/27/97
        IRIX)
            OS=irix`uname -r | sed 's/\..*//'`
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
    MACHINE=`uname -m | tr '[[A-Z]]' '[[a-z]]'`
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
                ip2?)
                    MACHINE=sgi
                    ;;
            esac
            ;;
	ultrix*)
	    case $MACHINE in
		vax*)
		     case "$CC" in
        		/bin/cc*|cc*)
echo "changing C compiler to \`vcc' because \`cc' floating-point is broken"
            		CC=vcc
            		;;
		     esac
		     ;;
	    esac
	    ;;

    esac
    fi

    AC_SUBST(MACHINE)
    AC_MSG_RESULT($MACHINE)])

AC_DEFUN(DODS_CHECK_SIZES, [dnl
    # Ignore the errors about AC_TRY_RUN missing an argument. jhrg 5/2/95

    AC_REQUIRE([AC_PROG_CC])

    AC_CHECK_SIZEOF(int)
    AC_CHECK_SIZEOF(long)
    AC_CHECK_SIZEOF(char)
    AC_CHECK_SIZEOF(double)
    AC_CHECK_SIZEOF(float)

    # Now generate symbols that define the dods_int32, ..., types
    # based on this machine's notion of an int, etc. See dods-datatypes.h.in.
    # I've separated the typedefs from the config_dap.h header because other
    # projects which use the DAP were getting conflicts with their includes,
    # or the includes of still other libraries, and config_dap.h. The 
    # config_dap.h header is now included only by .cc and .c files and headers
    # that need the typedefs use dods-datatypes.h. The code below makes 
    # dods-datatypes.h stand on its own. 8/2/2000 jhrg

    if test $ac_cv_sizeof_long -eq 4 
    then
	DODS_INT32=long
	DODS_UINT32="unsigned long"
	XDR_INT32=xdr_long
	XDR_UINT32=xdr_u_long
    elif test $ac_cv_sizeof_int -eq 4
    then
	DODS_INT32=int
	DODS_UINT32="unsigned int"
	XDR_INT32=xdr_int
	XDR_UINT32=xdr_u_int
    else
	AC_MSG_ERROR(How do I get a 32 bit integer on this machine?)
    fi
    AC_SUBST(DODS_INT32)
    AC_SUBST(DODS_UINT32)
    AC_SUBST(XDR_INT32)
    AC_SUBST(XDR_UINT32)

    # Assume nobody's hosed the 16-bit types...
    DODS_INT16=short
    DODS_UINT16="unsigned short"
    XDR_INT16=xdr_short
    XDR_UINT16=xdr_u_short
    AC_SUBST(DODS_INT16)
    AC_SUBST(DODS_UINT16)
    AC_SUBST(XDR_INT16)
    AC_SUBST(XDR_UINT16)

    if test $ac_cv_sizeof_char -eq 1
    then
	DODS_BYTE="unsigned char"
    else
	AC_MSG_ERROR(How do I get an 8 bit unsigned integer on this machine?)
    fi
    AC_SUBST(DODS_BYTE)

    if test $ac_cv_sizeof_double -eq 8
    then
	DODS_FLOAT64=double
	DODS_FLOAT32=float
	XDR_FLOAT64=xdr_double
	XDR_FLOAT32=xdr_float
    else
	AC_MSG_ERROR(How do I get floating point types on this machine?)
    fi
    AC_SUBST(DODS_FLOAT64)
    AC_SUBST(DODS_FLOAT32)
    AC_SUBST(XDR_FLOAT64)
    AC_SUBST(XDR_FLOAT32)])
