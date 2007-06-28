
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

# (c) COPYRIGHT URI/MIT 1997
# Please read the full copyright statement in the file COPYRIGHT_URI.
#
# Authors:
#      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

# Clean the cache directory. Removed oldest files until $cache contains less
# than cache_max_size megabytes.
#
# Example use:
# $cache_size = purge_cache(".", 30);
# print "cache now occupies $cache_size bytes\n";

package DODS_Cache;
require Exporter;
@ISA    = qw(Exporter);
@EXPORT = qw(purge_cache is_compressed decompress_and_cache);

my $test  = 0;
my $debug = 0;

# This regex is used to recognize files that are compressed.
my $compressed_regex = "(\\.gz|\\.Z|\\.bz2)";

use strict;

# Manage the cache directory. Given the pathname to the cache directory and
# the max size (in GB) of the cache, purge files until the size of cached
# files falls below the max size. Use an `oldest first' purge strategy. The
# cache directory can be used by other programs; this routine removes only
# files added by the routine decompress_and_cache(). It does this by
# selecting files which contains he regex `dods_cache#'.
#
# The arguments to this subroutine are:
# CACHE: the directory that holds the cached files
# CACHE_MAX_SIZE: prune files from the cache until the size (in megabytes) of
# the cache is less than this value.

sub purge_cache {
    my ( $cache, $cache_max_size ) = @_;

    $cache_max_size *= 1048576;    # Bytes/Meg
    my $cached_names_regex = ".*dods_cache#.*";
    my $cache_size         = 0;

    # Read the cache directory contents.
    opendir CACHE, $cache or die "Could not open the cache directory!\n";
    my (@files) = grep( /$cached_names_regex/, readdir(CACHE) );
    closedir CACHE;

    my ( %links, %sizes, %times, $nlink, $size, $atime, $file, $d );
    foreach $file (@files) {
        $file = "$cache\/$file";

        ( $d, $d, $d, $nlink, $d, $d, $d, $size, $atime ) = stat($file);
        $links{$file} = $nlink;
        $sizes{$file} = $size;
        $times{$file} = $atime;
        $cache_size += $size;
    }

    print STDERR "Spool size: $cache_size\n"         if $debug;
    print STDERR "Spool max size: $cache_max_size\n" if $debug;

    # Remove oldest files first. Continue removing files until cache size
    # falls below max size. Note that for sort ``... the normal calling code
    # for subroutines is bypassed... .'' Look in Wall's ``Programming PERL'',
    # p.218. 7/10/2001 jhrg
    my (@files_sorted_by_times) =
      sort( { $times{$a} <=> $times{$b} } keys(%times) );

    print STDERR "files sorted by times: @files_sorted_by_times\n" if $debug;

    my $sorted_file;
    foreach $sorted_file (@files_sorted_by_times) {

        # If enough files have been removed, stop flushing the cache.
        if ( $cache_size < $cache_max_size ) {
            return $cache_size;
        }

        # Remove the file if only one link to it exists (i.e., not in use).
        # Then update cache_size.
        if ( $links{$sorted_file} == 1 ) {
            unlink $sorted_file;

            # print STDERR "Removing $sorted_file\n";
            $cache_size -= $sizes{$sorted_file};
        }
    }
}

# Return 1 if the filename looks like one we think is compressed, 0
# otherwise. Compressed files end with either .Z or .gz. Or .bz2
sub is_compressed {
    my $file_name = shift;

    return ( $file_name =~ m@.*$compressed_regex@ ) ? 1 : 0;
}

my $dods_ext = "(das|dds|dods|asc|ascii|html|info|ver)";

# Given the full path name to a compressed file, decompress that file and
# store the result in the cache. If the file already exists in the cache, do
# nothing. Return the name of the cache entity. Use this in place of the
# pathname to the compressed file. Before calling this routine, make sure
# $pathname has been sanitized!
sub decompress_and_cache {
    my $pathname  = shift;
    my $cache_dir = shift;

    # Strip shell meta-characters from the $pathname. 7/10/2001 jhrg
    print STDERR "pathname: $pathname\n" if $debug;
    $pathname !~ m@.*[\s%&()*?<>;]+.*@
      or return ( "", "Found shell meta characters in pathname" );

    my $cache_entity = cache_name( $pathname, $cache_dir );

    # Only uncompress and cache if the data file actually exits *and* has not
    # already been decompressed and cached. Add a test for zero-length cache 
    # entities. That's the same as the entity not existing. jhrg 11/25/05
    if ( ( !-e $cache_entity || -s $cache_entity == 0 ) && ( -e $pathname ) ) {

        # This code uses two opens and is a safer than using system since
        # $cache_entity is not run through the shell. However, it sounds like
        # (see ``Programming PERL'') that $pathanme is still exposed to the
        # shell since open uses /bin/sh when a pipe symbol is present.
        
        # Choose which filter to use for decompression. Don't put these in
        # the rc file since that file is stored in a CGI bin directory. If 
        # gzip and/or bzip2 are not in the standard places, edit this file.
        # I'm cautious about running just any copy of gzip/bzip2 for the same
        # reason those programs are not in the rc file.
        my $comp;
        if ($pathname =~ m@.*\.bz2@) {
        		if (-x "/bin/bzip2") {
        			$comp = "/bin/bzip2";
        		} 
        		elsif (-x "/usr/bin/bzip2") {
        			$comp = "/usr/bin/bzip2";
        		}
        		else {
        			return ("", "Could not locate the bzip2 decompression utility");
        		}
        }
        else {
        		if (-x "/bin/gzip") {
        			$comp = "/bin/gzip";
        		} 
        		elsif (-x "/usr/bin/gzip") {
        			$comp = "/usr/bin/gzip";
        		}
        		else {
        			return ("", "Could not locate the gzip decompression utility");
        		}
        }
        my $uncomp = $comp . " -c -d " . $pathname . " |";
        my $buf;
        open GZIP, $uncomp
          or return ( "",
            "Could not decompress $pathname: Unable to open the decompresser" );
        open DEST, ">$cache_entity"
          or return ( "",
                    "Could not decompress $pathname: Unable to open destination"
          );
        while ( read GZIP, $buf, 16384 ) {
            print DEST $buf;
        }

        close GZIP;
        close DEST;
    }

    return ( $cache_entity, "" );
}

# Private. Build up the cache filename.
sub cache_name {
    my $pathname  = shift;
    my $cache_dir = shift;

    $pathname =~ s@^/@@;                   # delete leading /
    $pathname =~ s@/@\#@g;                 # turn remaining / into #
    $pathname =~ s@$compressed_regex@@;    # delete trailing file type

    return $cache_dir . "/" . "dods_cache#" . $pathname;
}

# Private. Get the remote thing. The param $url should be scanned for shell
# meta-characters.
sub transfer_remote_file {
    my $url          = shift;
    my $cache_entity = shift;

    my $curl = "curl"; # bug: Should use value from dap-server.rc. jhrg 11/25/05

    my $transfer =
      $curl . " --silent --user anonymous:root\@dods.org " . $url . " |";
    my $buf;
    open CURL, $transfer
      or return ( "",
                 "Could not transfer $url: Unable to open the transfer utility."
      );
    open DEST, ">$cache_entity"
      or return (
        "",
"Could not transfer $url: Unable to open destination in the local cache."
      );

    my $bytes;
    my $total = 0;
    while ( $bytes = read CURL, $buf, 16384 ) {
        my $status = print DEST $buf;
        print( STDERR "Status: $status\n" ) if $debug > 0;
        if ( !$status ) {
            close CURL;
            close DEST;
            unlink $cache_entity;
            return ( $total,
                     "Error transferring remote file ($url) to the cache." );
        }
        $total += $bytes;
    }

    close CURL;
    close DEST;

    return ( $total, "" );
}

##########################################################################

if ($test) {
	if (0) {
    ( 1 == is_compressed("myfile.Z") )  || die;
    ( 1 == is_compressed("my.file.Z") ) || die;
    ( 1 == is_compressed("test.gz") )   || die;
    ( 0 == is_compressed("myfile.") )   || die;
    ( 0 == is_compressed("my.file") )   || die;
    ( 0 == is_compressed("test") )      || die;
    ( 0 == is_compressed("gz") )        || die;
    ( 0 == is_compressed("bz2") )        || die;
    ( 1 == is_compressed("myfile.bz2") ) || die;
    ( 1 == is_compressed("myfile.hdf.bz2") ) || die;
    print "\t is_compressed passed all tests\n";

    # NB: Never call cache_name with "" for the cache directory. I'm doing it
    # here just to test stuff. 10/18/2000 jhrg
    print "cache_name myfile: ", cache_name( "myfile", "" ), "\n"
      if $debug;
    ( "/dods_cache#myfile" eq cache_name( "myfile",    "" ) ) || die;
    ( "/dods_cache#myfile" eq cache_name( "myfile.Z",  "" ) ) || die;
    ( "/dods_cache#myfile" eq cache_name( "myfile.gz", "" ) ) || die;
    ( "/dods_cache#myfile" eq cache_name( "myfile.bz2", "" ) ) || die;
    print "cache_name ./myfile.gz", cache_name( "./myfile.gz", "" ), "\n"
      if $debug;
    ( "/dods_cache#this#is#myfile" eq cache_name( "/this/is/myfile.Z", "" ) )
      || die;
    ( "/usr/tmp/dods_cache#myfile" eq cache_name( "myfile", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#myfile" eq cache_name( "myfile.Z", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#myfile" eq cache_name( "myfile.gz", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#myfile" eq cache_name( "myfile.gz", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#this#is#myfile" eq
       cache_name( "/this/is/myfile.bz2", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#this#is#myfile.HDF" eq
       cache_name( "/this/is/myfile.HDF.Z", "/usr/tmp" ) )
      || die;
    ( "/usr/tmp/dods_cache#this#is#my.file.HDF" eq
       cache_name( "/this/is/my.file.HDF.Z", "/usr/tmp" ) )
      || die;
    print "\t cache_name passed all tests\n";

	}

    ( $a, $b ) = decompress_and_cache( "test_file.gz", "/tmp" );
    print "decompressed test_file: " . $a . "\n" if $debug;
    print "error message: " . $b . "\n" if $debug;
    
    ( "/tmp/dods_cache#test_file" eq $a && "" eq $b ) || die;
    ( -e "/tmp/dods_cache#test_file" )       || die;
    ( 55 == -s "/tmp/dods_cache#test_file" ) || die;

    ( $a, $b ) = decompress_and_cache( "test/test_file.gz", "/tmp" );
    print "decompressed test_file: " . $a . "\n" if $debug;
    print "error message: " . $b . "\n" if $debug;
    
    ( "/tmp/dods_cache#test#test_file" eq $a && "" eq $b ) || die;
    ( -e "/tmp/dods_cache#test#test_file" )  || die;
    ( 55 == -s "/tmp/dods_cache#test_file" ) || die;

    ( $a, $b ) = decompress_and_cache( "test_file2.bz2", "/tmp" );
    print "decompressed test_file: " . $a . "\n" if $debug;
    print "error message: " . $b . "\n" if $debug;
    
    ( "/tmp/dods_cache#test_file2" eq $a && "" eq $b ) || die;
    ( -e "/tmp/dods_cache#test_file2" )       || die;
    ( 55 == -s "/tmp/dods_cache#test_file2" ) || die;

    ( $a, $b ) = decompress_and_cache( "test/test_file2.bz2", "/tmp" );
    ( "/tmp/dods_cache#test#test_file2" eq $a && "" eq $b ) || die;
    ( -e "/tmp/dods_cache#test#test_file2" )  || die;
    ( 55 == -s "/tmp/dods_cache#test_file2" ) || die;
    
    # Test scanning for meta chars before sending pathname to /bin/sh.
    ( $a, $b ) =
      decompress_and_cache( "test_file.gz%3Bcat%20/etc/passwd%3B", "/usr/tmp" );
    ( "" eq $a && "" ne $b ) || die;    # Any error message passes the test.

    print "\t decompress_and_cache passed all tests\n";

    # Test the transfer_remote_file() function

    transfer_remote_file( "http://test.opendap.org/data/nc/fnoc1.nc",
                          "/tmp/dods_http_fnoc1.nc" );
    ( -e "/tmp/dods_http_fnoc1.nc" && 23944 == -s "/tmp/dods_http_fnoc1.nc" )
      || die;

    unlink "/tmp/dods_http_fnoc1.nc";

    #    unlink "/tmp/dods_ftp_fnoc1.nc";

    print "\t transfer_remote_file passed all tests\n";

    ( purge_cache( "/tmp", 1 ) > 0 ) || die;
    ( 0 == purge_cache( "/tmp", 0 ) ) || die;
    print "\t purge_cache passed all tests\n";

    print "All tests succeeded\n";
}

1;

# $Log: DODS_Cache.pm,v $
# Revision 1.10  2005/05/18 21:33:16  jimg
# Update for the new build/install.
#
# Revision 1.9  2004/01/22 17:29:37  jimg
# Merged with release-3-4.
#
# Revision 1.8.4.1  2003/06/19 18:58:24  jimg
# Fixed the use of curl. It was expecting to find curl in the bin directory.
# That broke sites that used the installServers script (which they all should
# be doing!). Now this code expects to find curl in the same place as all the
# other server executables.
#
# Revision 1.8  2003/01/23 00:44:34  jimg
# Updated the copyrights on various source files. OPeNDAP is adopting the
# GNU Lesser GPL.
#
# Revision 1.7  2003/01/22 00:12:05  jimg
# Added/Updated from release-3-2 branch.
#
# Revision 1.6  2002/12/31 22:28:45  jimg
# Merged with release 3.2.10.
#
# Revision 1.4.2.7  2002/11/05 00:38:32  jimg
# DODSter fixes.
#
# Revision 1.4.2.6  2002/10/24 01:03:11  jimg
# I improved the call to curl so that it uses the copy in ../bin if it's
# there, otherwise it uses the copy on PATH.
#
# Revision 1.4.2.5  2002/10/24 00:41:07  jimg
# Added dodster code.
#
# Revision 1.4.2.4  2002/05/26 23:10:00  jimg
# I removed tests for the Progress Indicator from the configure.in and
# configure scripts. Also removed was the libdap++-gui.a library and any
# use of it by the Makefile.in templates. Finally, the src/packages/libtcl
# and libtk directories were removed (from the cvs repository; they are
# in the src/packages/Attic directory). This removes all vestiges of the
# tcl/tk based progress indicator from our code.
#
# Revision 1.4.2.3  2001/10/14 00:42:32  jimg
# Merged with release-3-2-8
#
# Revision 1.4.2.2  2001/07/11 05:08:40  jimg
# Fixed a security problem (Bug 186) where pathnames with shell meta characters
# could make their way to /bin/sh (if they were present in pathnames to be
# decompressed. The shell is still used (haven't figured out how to run gzip in
# the server without redirecting stdout) but paths are sanitized.
# I also fixed a problem with the used of sort. Perl was spitting out a
# warning because the ordering function used by sort used a variable that was
# out of scope.
#
# Revision 1.5  2001/06/15 23:38:36  jimg
# Merged with release-3-2-4.
#
# Revision 1.4.2.1  2001/06/15 17:20:03  jimg
# Added comment about a potential security hole.
#
# Revision 1.4  2000/10/19 17:17:10  jimg
# Added functions to complete the caching system. It is still pretty basic...
#
# Revision 1.3  1999/05/28 05:37:11  jimg
# Merge with alpha-3-0-0
#
# Revision 1.2.26.1  1999/05/27 19:59:53  rmorris
# Refer to cache files using the full path instead a relative path so that
# failure to "be in" the right directory during purging of cache files
# doesn't make cache purging fail and bring down the server when the
# file system fills.   This only effects the hdf server.
#
# Revision 1.2  1997/10/09 22:19:17  jimg
# Resolved conflicts in merge of 2.14c to trunk.
#
# Revision 1.1  1997/09/30 22:16:50  jimg
# Created. Initially for use with the JPL Pathfinder server.
