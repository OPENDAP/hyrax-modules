#!/bin/sh
#
# Check that the perl scripts do not have debugging turned on.

for script in *.pm
do
    if ! grep '\$debug = 0' $script >/dev/null 2>&1 \
	|| grep '\$debug = [1-9]' $script >/dev/null 2>&1
    then
	exit 1;
    fi
done

exit 0;
