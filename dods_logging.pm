
# -*- perl -*-

# This file is part of libdap, A C++ implmentation of the OPeNDAP Data
# Access Protocol.

# Copyright (c) 2002,2003 OPeNDAP, Inc.
# Author: James Gallagher <jgallagher@opendap.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

# Send DODS loggin information to stdout. This will be run in response to a
# status request to the server dispatch code (nph-dods).

package dods_logging;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(print_log_info);

use strict;

my $test = 0;

sub print_log_info {
    my $access_log_location=shift;
    my $error_log_location=shift;

    print "Access information:\n";
    open ACCESS, $access_log_location or print "Could not open the access log.\n";

    while(<ACCESS>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "Error information:\n";
    open ERROR, $error_log_location or print "Could not open the error log.\n";

    while(<ERROR>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "End of log information\n";
}

if ($test) {
    &print_log_info("/var/log/httpd/access_log", "/var/log/httpd/error_log");
    &print_log_info("/var/log/httpd/access_log", "/dev/null");
    &print_log_info("/var/log/httpd/access_log", "/no/such/file.ever");
}

1;

# $Log: dods_logging.pm,v $
# Revision 1.4  2004/01/22 17:29:37  jimg
# Merged with release-3-4.
#
# Revision 1.3.4.3  2004/01/12 20:53:39  jimg
# Added error reporting so we can tell if a site has turned stats on but
# blocked access to the actual log files.
#
# Revision 1.3.4.2  2003/06/18 18:34:12  jimg
# Dan reports a problem compiling this file; DODS_Dispatch.pm gripes about the
# load of this file not returning true. I added a '1;' to its end.
#
# Revision 1.3.4.1  2003/06/18 05:39:44  jimg
# Fixed bug 510: The script requied that both access_ and error_log be set.
# People wanted to set only one (access_log) and that was returning an error.
# I also addedd a small bit of test code.
#
# Revision 1.3  2003/01/23 00:44:34  jimg
# Updated the copyrights on various source files. OPeNDAP is adopting the
# GNU Lesser GPL.
#
# Revision 1.2  2002/12/31 22:26:45  jimg
# Added.
#
