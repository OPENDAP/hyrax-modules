
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

# package handler_name;
package read_config;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(handler_name dataset_regexes get_params);

use strict;

my $debug = 0;
my $test = 0;

# Read the values of various configuration parameters. Return a list with the
# following configuration parameters: ($timeout, $cache_dir, $cache_size,
# $maintainer, @exclude). Because the last element is a list, it's best to
# extract the first elements using shift or a list assignment.

sub get_params {
    my ($server_config_file) = @_;
    my $timeout = 0;		# Never timeout
    my $cache_dir = "/usr/tmp";	# Default
    my $maintainer = "support\@unidata.ucar.edu"; # Default
    my $cache_size = 50;	# Default
    my @exclude;		# Default is empty

    open (DODSINI, $server_config_file);

  LINE: 
    while (<DODSINI>) {
	next LINE if /^\#/;
	next LINE if /^$/;

	my ($keyword, @value) = split;

	print STDERR "Keyword: $keyword, Value: @value\n" 
	    if $debug >= 2;

	if ($keyword eq "timeout") {
	    $value[0] =~ /^(.*)$/;
	    $timeout = $1;
	}
	elsif ($keyword eq "cache_dir") {
	    print STDERR "Cache Dir: $value[0]\n" if $debug;
	    $value[0] =~ /^(.*)$/;
	    $cache_dir = $1;
	    print STDERR "Cache Dir: $cache_dir\n" if $debug;
	}
	elsif ($keyword eq "cache_size") {
	    $value[0] =~ /^(.*)$/;
	    $cache_size = $1
	}
	elsif ($keyword eq "maintainer") {
	    $value[0] =~ /^(.*)$/;
	    $maintainer = $1;
	    # $value[0] is an email address; if not already done, we must
	    # escape the @ sign. 
	    if ($maintainer !~ /\\\@/) {
		$maintainer =~ s/\@/\\\@/;
	    }
	}
	elsif ($keyword eq "exclude") {
	    # These are only used internally, no need to sanitize (which is
	    # good because I can't figure out how to sanitize a list...)
	    # 07/23/03 jhrg
	    @exclude = @value;
	}
    }

    return ($timeout, $cache_dir, $cache_size, $maintainer, @exclude);
}

# Lookup the handler name using the regular expressions from the dods.rc
# file. The dods.rc file contains lines with one regular expression followed
# by the handler name token. Comment start with the `#' character. If the
# string (usually a pathname) matches a regular expression `, this function
# returns the handler associated with that regular expression. If no regexes
# match, then return "".

sub handler_name {
    my ($pathname, $server_config_file) = @_;

    print STDERR "Pathanme: ", $pathname, "\n" if $debug;

    open (DODSINI, $server_config_file);

  LINE: 
    while (<DODSINI>) {
	next LINE if /^\#/;
	next LINE if /^$/;
	next LINE if ! /^handler/;

	my ($keyword, $regex, $handler) = split;

	print STDERR "Keyword: $keyword, Regex: $regex, Handler: $handler \n" 
	    if $debug >= 2;

	if ($pathname =~ $regex) {
	    print STDERR "match $handler\n" if $debug >= 1;
	    # Sanitize data read from configuration file. 04/28/03 jhrg
	    $handler =~ /^([\w]+)$/;
	    return $1;
	}
    }

    return "";
}

# Collect the regexes that describe the files this server is configured to
# recognize as data. Return the regexes in a list. Exclude those that
# are used for non file-based data sources (like JGOFS). 
#
# server_config_file: the name of the DODS server configuration file.
# exclude: a list of handlers whose regexes should be excluded from the
# returned list.

sub dataset_regexes {
    my ($server_config_file, @exclude) = @_;
    my @regexes;			# Used for the return value.

    open (DODSINI, $server_config_file);

  LINE: 
    while (<DODSINI>) {
	next LINE if /^\#/;
	next LINE if /^$/;
	next LINE if ! /^handler/;

	my ($keyword, $regex, $handler) = split;

	# Note that I'm using $_ explicitly since that contains the pattern I
	# want to test. Usually you don't need to do this. In most cases,
	# $handler is the pattern and @exclude holds the things to be
	# matched. In this case, however, @exclude holds the patterns and I
	# want to see if any of them match the string $handler. The same
	# problem could be solved by concatenating a bunch of patterns with
	# logical ORs, but the resulting pattern could get really large.
	if (!grep {$handler =~ $_} @exclude) {
	    print STDERR "Including $regex\n" if $debug >= 1;
	    @regexes = (@regexes, $regex);
	}
    }

    return @regexes;
}

# Tests
if ($test) {
    ("hdf" eq handler_name("/stuff/file.HDF", "./dods.rc")) || die;
    ("hdf" eq handler_name("/stuff/file.hdf", "./dods.rc")) || die;
    ("nc" eq handler_name("/stuff/file.nc", "./dods.rc")) || die;
    ("nc" eq handler_name("/stuff/file.NC", "./dods.rc")) || die;
    ("nc" eq handler_name("/stuff/file.cdf", "./dods.rc")) || die;
    ("nc" eq handler_name("/stuff/file.CDF", "./dods.rc")) || die;
    ("jg" eq handler_name("/stuff/test", "./dods.rc")) || die;

    ("mat" eq handler_name("/stuff/test.mat", "./dods.rc")) || die;
    ("mat" eq handler_name("/stuff/test.Mat", "./dods.rc")) || die;
    ("mat" eq handler_name("/stuff/test.MAT", "./dods.rc")) || die;
    ("ff" eq handler_name("/stuff/test.dat", "./dods.rc")) || die;
    ("ff" eq handler_name("/stuff/test.bin", "./dods.rc")) || die;
    ("" eq handler_name("/stuff/file.bob", "./dods.rc")) || die;
    ("hdf" eq 
     handler_name("/usr/tmp/dods_cache#home#httpd#html#data#hdf#S1700101.HDF",
		  "./dods.rc")) || die;
    
    print STDERR "first: ", dataset_regexes("./dods.rc", ("hdf", "nc", "mat",
					       "ff", "dsp", "jg")), "\n";
    print STDERR "second: ", dataset_regexes("./dods.rc", ("nc", "mat", "ff",
					       "dsp", "jg")), "\n";
    print STDERR "third: ", dataset_regexes("./dods.rc", ("jg")), "\n";

    my @params = get_params("./dods.rc"); 
    print STDERR "params: @params\n" if $debug;
    ($params[0] == 0) || die; shift @params;
    ($params[0] == "/usr/tmp") || die; shift @params;
    ($params[0] == 50) || die; shift @params;
    ($params[0] == "support\@unidata.ucar.edu") || die; shift @params;
    print STDERR "Exclude: @params\n";
    my @exclude_test = ();
    (@params == @exclude_test) || die;

    print "All tests successful.\n";
}

1;

# $Log: read_config.pm,v $
# Revision 1.2  2004/01/22 17:29:37  jimg
# Merged with release-3-4.
#
# Revision 1.1.2.2  2003/11/21 19:27:00  jimg
# Fixed two lines of instrumentation so they trigger only when $debug is
# non-zero.
#
# Revision 1.1.2.1  2003/07/23 23:24:32  jimg
# Moved from handler_name.pm when I made the dods.rc a real configuration
# file.
#
# Revision 1.8  2003/04/28 22:01:47  jimg
# Data (the handler name) read from the configuration file is tainted and must
# be sanitized.
#
# Revision 1.7  2003/01/23 00:44:34  jimg
# Updated the copyrights on various source files. OPeNDAP is adopting the
# GNU Lesser GPL.
#
# Revision 1.6  2003/01/22 00:41:47  jimg
# Changed dods.ini to dods.rc.
#
# Revision 1.5  2002/12/31 22:28:45  jimg
# Merged with release 3.2.10.
#
# Revision 1.3.4.4  2002/10/24 00:44:31  jimg
# Fixed up some of the comments.
#
# Revision 1.3.4.3  2001/05/15 23:18:57  jimg
# Added some testing code.
#
# Revision 1.3.4.2  2001/05/09 21:34:34  jimg
# Fixed an error in the EXPORT line.
#
# Revision 1.3.4.1  2001/05/09 21:33:41  jimg
# Added dataset_regexes(...). This returns a list of regular expressions which
# the server is configured to recognize as datasets. There's a way to exclude
# some of the expressions. The routine does not have any automated tests yet,
# but it should...
#
# Revision 1.3  2000/10/19 23:47:09  jimg
# Added more tests and a debug switch.
#
# Revision 1.2  2000/10/18 22:40:17  jimg
# Added the config file as a parameter. This will provide a way for the
# dispatch script to pass in the config file's name.
#
# Revision 1.1  2000/10/18 22:08:01  jimg
# Added
#
