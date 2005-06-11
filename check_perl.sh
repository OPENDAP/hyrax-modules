#!/bin/sh
#
# Check that the perl scripts do not have debugging turned on.

files_w_debug_on="`grep -l '^my.*\$debug.*=.*[1-9].*;$' *.pm`"

if test "$files_w_debug_on"; then
    exit 1;
fi

exit 0;
