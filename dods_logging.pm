
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

sub print_log_info {
    my $access_log_location=shift;
    my $error_log_location=shift;

    print "Access information:\n";
    open ACCESS, $access_log_location 
	or die "Could not open the access log file ($access_log_location)\n";
    while(<ACCESS>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "Error information:\n";
    open ERROR, $error_log_location 
	or die "Could not open the error log file ($error_log_location)\n";
    while(<ERROR>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "End of log information\n";
}

# &print_log_info("/var/log/httpd/access_log", "/var/log/httpd/error_log");

# $Log: dods_logging.pm,v $
# Revision 1.3  2003/01/23 00:44:34  jimg
# Updated the copyrights on various source files. OPeNDAP is adopting the
# GNU Lesser GPL.
#
# Revision 1.2  2002/12/31 22:26:45  jimg
# Added.
#
