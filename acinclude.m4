# -*- autoconf -*-
#
# autoconf macros. 
#

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

