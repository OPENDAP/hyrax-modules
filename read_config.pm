
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
@ISA    = qw(Exporter);
@EXPORT = qw(handler_name dataset_regexes get_params);

use strict;

# The test code make many assumptions about your installation.
# In particular, it assumes a 'default' installation of the server.
my $test  = 0;

# Read the values of various configuration parameters. Return a list with the
# following configuration parameters: ($timeout, $cache_dir, $cache_size,
# $maintainer, @exclude). Because the last element is a list, it's best to
# extract the first elements using shift or a list assignment.

sub get_params {
    my ($server_config_file) = @_;
    my $timeout              = 0;                             # Never timeout
    my $cache_dir            = "/usr/tmp";
    my $maintainer           = "support\@unidata.ucar.edu";
    my $cache_size           = 50;
    my $curl                 = "curl";
    my $usage                = "dap_usage";
    my $www_int              = "dap_www_int";
    my $asciival             = "dap_asciival";
    my $data_root            = "";
    my @exclude;                                              # Default is empty
    my $error = "";

    open( DODSINI, $server_config_file )
      or $error = "OPeNDAP server: Could not open the configuarion file.";

  LINE:
    while (<DODSINI>) {
        next LINE if /^\#/;
        next LINE if /^$/;

        my ( $keyword, @value ) = split;

        if ( $keyword eq "timeout" ) {
            $value[0] =~ /^(.*)$/;
            $timeout = $1;
        }
        elsif ( $keyword eq "cache_dir" ) {
            $value[0] =~ /^(.*)$/;
            $cache_dir = $1;
        }
        elsif ( $keyword eq "cache_size" ) {
            $value[0] =~ /^(.*)$/;
            $cache_size = $1;
        }
        elsif ( $keyword eq "maintainer" ) {
            $value[0] =~ /^(.*)$/;
            $maintainer = $1;

            # $value[0] is an email address; if not already done, we must
            # escape the @ sign.
            if ( $maintainer !~ /\\\@/ ) {
                $maintainer =~ s/\@/\\\@/;
            }
        }
        elsif ( $keyword eq "curl" ) {
            $value[0] =~ /^(.*)$/;
            $curl = $1;
        }
        elsif ( $keyword eq "usage" ) {
            $value[0] =~ /^(.*)$/;
            $usage = $1;
        }
        elsif ( $keyword eq "www_int" ) {
            $value[0] =~ /^(.*)$/;
            $www_int = $1;
        }
        elsif ( $keyword eq "asciival" ) {
            $value[0] =~ /^(.*)$/;
            $asciival = $1;
        }
        elsif ( $keyword eq "data_root" ) {
            $value[0] =~ /^(.*)$/;
            $data_root = $1;
        }
        elsif ( $keyword eq "exclude" ) {

            # These are only used internally, no need to sanitize (which is
            # good because I can't figure out how to sanitize a list...)
            # 07/23/03 jhrg
            @exclude = @value;
        }
    }

    return (
        $timeout,   $cache_dir, $cache_size, $maintainer,
        $curl,      $usage,     $www_int,    $asciival,
        $data_root, $error,     @exclude
    );
}

# Lookup the handler name using the regular expressions from the
# dap-server.rc file. The dap-server.rc file contains lines with one regular
# expression followed by the handler name token. Comments start with the `#'
# character. If the string (usually a pathname) matches a regular expression,
# this function returns the handler associated with that regular expression.
# If no regexes match, then return "".

sub handler_name {
    my ( $pathname, $server_config_file ) = @_;

    open( DODSINI, $server_config_file );

  LINE:
    while (<DODSINI>) {
        next LINE if /^\#/;
        next LINE if /^$/;
        next LINE if !/^handler/;

        my ( $keyword, $regex, $handler, $switches ) = split /\s+/, $_, 4;

        if ( $pathname =~ $regex ) {
            # Sanitize data read from configuration file. 04/28/03 jhrg
            $handler =~ /^([^#!:;]+)$/;
            $handler = $1;

            # Make sure to sanitise $switches even if it's null.
            if ( $switches eq "" ) {
                $switches = "";
            }
            else {
                $switches =~ /^([^#!:;\n]+.*)$/;
                $switches = $1;
                # chop $switches;
            }

            return ( $handler , $switches );
        }
    }

    return ( "", "" );
}

# Collect the regexes that describe the files this server is configured to
# recognize as data. Return the regexes in a list. Exclude those that
# are used for non file-based data sources (like JGOFS).
#
# server_config_file: the name of the DODS server configuration file.
# exclude: a list of handlers whose regexes should be excluded from the
# returned list.

sub dataset_regexes {
    my ( $server_config_file, @exclude ) = @_;
    my @regexes;    # Used for the return value.

    open( DODSINI, $server_config_file );

  LINE:
    while (<DODSINI>) {
        next LINE if /^\#/;
        next LINE if /^$/;
        next LINE if !/^handler/;

        my ( $keyword, $regex, $handler ) = split;

        # Note that I'm using $_ explicitly since that contains the pattern I
        # want to test. Usually you don't need to do this. In most cases,
        # $handler is the pattern and @exclude holds the things to be
        # matched. In this case, however, @exclude holds the patterns and I
        # want to see if any of them match the string $handler. The same
        # problem could be solved by concatenating a bunch of patterns with
        # logical ORs, but the resulting pattern could get really large.
        if ( !grep { $handler =~ $_ } @exclude ) {
            @regexes = ( @regexes, $regex );
        }
    }

    return @regexes;
}

# Tests
#
# These tests work only if you use the default values in the dap-server.rc.

if ($test) {
    my ($handler, $switches) = handler_name( "/stuff/file.nc", "./dap-server.rc" );
    print "Handler: $handler\n";
    print "Switches: $switches\n";

    ( "/usr/local/bin/dap_hdf4_handler" eq
          handler_name( "/stuff/file.HDF", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_hdf4_handler" eq
          handler_name( "/stuff/file.hdf", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_nc_handler" eq
          handler_name( "/stuff/file.nc", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_nc_handler" eq
          handler_name( "/stuff/file.NC", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_nc_handler" eq
          handler_name( "/stuff/file.cdf", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_nc_handler" eq
          handler_name( "/stuff/file.CDF", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_ff_handler" eq
          handler_name( "/stuff/test.dat", "./dap-server.rc" ) )
      || die;
    ( "/usr/local/bin/dap_ff_handler" eq
          handler_name( "/stuff/test.bin", "./dap-server.rc" ) )
      || die;
    ( "" eq handler_name( "/stuff/file.bob", "./dap-server.rc" ) ) || die;
    (
        "/usr/local/bin/dap_hdf4_handler" eq handler_name(
            "/usr/tmp/dods_cache#home#httpd#html#data#hdf#S1700101.HDF",
            "./dap-server.rc"
        )
      )
      || die;

    print STDERR "first: ",
      dataset_regexes(
        "./dap-server.rc", ( "hdf", "nc", "mat", "ff", "dsp", "jg" )
      ),
      "\n";
    print STDERR "second: ",
      dataset_regexes( "./dap-server.rc", ( "nc", "mat", "ff", "dsp", "jg" ) ),
      "\n";
    print STDERR "third: ", dataset_regexes( "./dap-server.rc", ("jg") ), "\n";

    my @params = get_params("./dap-server.rc");
    print STDERR "params: @params\n";
    ( $params[0] == 0 ) || die;
    shift @params;
    ( $params[0] == "/usr/tmp" ) || die;
    shift @params;
    ( $params[0] == 500 ) || die;
    shift @params;
    ( $params[0] == "support\@unidata.ucar.edu" ) || die;
    shift @params;
    ( $params[0] == "curl" ) || die;
    shift @params;
    ( $params[0] == "" ) || die;
    shift @params;
    print STDERR "Exclude: @params\n";
    my @exclude_test = ();
    ( @params == @exclude_test ) || die;

    print "All tests successful.\n";
}

1;

