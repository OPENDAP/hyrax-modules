
# (c) COPYRIGHT URI/MIT 1997
# Please read the full copyright statement in the file COPYRIGH.  
#
# Authors:
#      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
#
# Clean the cache directory. Removed oldest files until $cache contains less
# than cache_max_size megabytes.
#
# Example use:
# $cache_size = purge_cache(".", 30);
# print "cache now occupies $cache_size bytes\n";

package DODS_Cache;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(purge_cache is_compressed decompress_and_cache);

my $test = 0;
my $debug = 0;

# This regex is used to recognize files that are compressed.
my $compressed_regex = "(\.gz|\.Z)";

sub reverse_by_times {
    ($times{$a} <=> $times{$b}) * -1;
}

sub by_times {
    $times{$a} <=> $times{$b};
}

use strict;

# Manage the cache directory. Given the pathname to the cache directory and
# the max size (in GB) of the cache, purge files until the size of cached
# files falls below the max size. Use an `oldest first' purge strategy. The
# cache directory can be used by other programs; this routine removes only
# files added by the routine decompress_and_cache(). It does this by
# selecting files which containt he regex `dods_cache#'.
#
# The arguments to this subroutine are: 
# CACHE: the directory that holds the cached files
# CACHE_MAX_SIZE: prune files from the cache until the size (in megabytes) of
# the cache is less than this value.

sub purge_cache {
    my($cache, $cache_max_size) = @_;

    $cache_max_size *= 1048576;	# Bytes/Meg
    my $cached_names_regex = ".*dods_cache#.*";
    my $cache_size = 0;
    
    # Read the cache directory contents.
    opendir(CACHE, $cache) || die "Could not open the cache directory!\n";
    my(@files) = grep(/$cached_names_regex/, readdir(CACHE));
    closedir(CACHE);

    my(%links, %sizes, %times, $nlink, $size, $atime, $file, $d);
    foreach $file (@files) {
        $file = "$cache\/$file";

	($d, $d, $d, $nlink, $d, $d, $d, $size, $atime) = stat($file);
	$links{$file} = $nlink;
	$sizes{$file} = $size;
	$times{$file} = $atime;
	$cache_size += $size;
    }

    print STDERR "Spool size: $cache_size\n" if $debug;
    print STDERR "Spool max size: $cache_max_size\n" if $debug;

    # Remove oldest files first. Continue removing files until cache size
    # falls below max size. 
    my(@files_sorted_by_times) = sort(by_times keys(%times));
    
    my $sorted_file;
    foreach $sorted_file (@files_sorted_by_times) {
	# If enough files have been removed, stop flushing the cache.
	if ($cache_size < $cache_max_size) {
	    return $cache_size;
	}
	# Remove the file if only one link to it exists (i.e., not in use).
	# Then update cache_size.
	if ($links{$sorted_file} == 1) {
	    unlink $sorted_file;
	    # print STDERR "Removing $sorted_file\n";
	    $cache_size -= $sizes{$sorted_file};
	}
    }
}

# Return 1 if the filename looks like one we think is compressed, 0
# otherwise. Compressed files end with either .Z or .gz.
sub is_compressed {
    my $file_name = shift;

    return ($file_name =~ m/.*$compressed_regex/) ? 1 : 0;
}

# Given the full path name to a compressed file, decompress that file and
# store the result in the cache. If the file already exists in the cache, do
# nothing. Return the name of the cache entity. Use this in place of the
# pathname to the compressed file. Before calling this routine, make sure
# $pathname has been sanitized!
sub decompress_and_cache {
    my $pathname = shift;
    my $cache_dir = shift;

    my $cache_entity = cache_name($pathname, $cache_dir);

    # Only uncompress and cache if the data file actually exits *and* has not
    # already been decompressed and cached.
    if ((! -e $cache_entity) && (-e $pathname)) { 
	my $uncomp = "gzip -c -d " . $pathname . " > " .  $cache_entity;
	system($uncomp); # uncompress and put output to cache
    }

    return $cache_entity;
}

# Private. Build up the cache filename.
sub cache_name {
    my $pathname = shift;
    my $cache_dir = shift;

    $pathname =~ s@^/@@;  # delete leading /
    $pathname =~ s@/@\#@g; # turn remaining / into #
    $pathname =~ s@$compressed_regex@@; # delete trailing filetype

    return $cache_dir . "/" . "dods_cache#" . $pathname;
}

if ($test) {
    (1 == is_compressed("myfile.Z")) || die;
    (1 == is_compressed("my.file.Z")) || die;
    (1 == is_compressed("test.gz")) || die;
    (0 == is_compressed("myfile.")) || die;
    (0 == is_compressed("my.file")) || die;
    (0 == is_compressed("test")) || die;
    (0 == is_compressed("gz")) || die;
    print "\t is_compressed passed all tests\n";

    # NB: Never call cache_name with "" for the cache directory. I'm doing it
    # here just to test stuff. 10/18/2000 jhrg
    print "cache_name myfile: ", cache_name("myfile", ""), "\n" 
  	if $debug; 
    ("/dods_cache#myfile" eq cache_name("myfile", "")) || die;
    ("/dods_cache#myfile" eq cache_name("myfile.Z", "")) || die;
    ("/dods_cache#myfile" eq cache_name("myfile.gz", "")) || die;
    print "cache_name ./myfile.gz", cache_name("./myfile.gz", ""), "\n" 
  	if $debug; 
    ("/dods_cache#this#is#myfile" eq cache_name("/this/is/myfile.Z", "")) || die;
    ("/usr/tmp/dods_cache#myfile" eq cache_name("myfile", "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#myfile" eq cache_name("myfile.Z", "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#myfile" eq cache_name("myfile.gz", "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#this#is#myfile" eq cache_name("/this/is/myfile.Z",
					     "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#this#is#myfile.HDF" eq cache_name("/this/is/myfile.HDF.Z",
					     "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#this#is#my.file.HDF" eq cache_name("/this/is/my.file.HDF.Z",
					     "/usr/tmp")) || die;
    print "\t cache_name passed all tests\n";

    ("/usr/tmp/dods_cache#test_file" eq decompress_and_cache("test_file.gz",
						  "/usr/tmp")) || die;
    (-e "/usr/tmp/dods_cache#test_file") || die;
    (89 == -s "/usr/tmp/dods_cache#test_file") || die;
    ("/usr/tmp/dods_cache#test_file" eq decompress_and_cache("test_file.gz",
						  "/usr/tmp")) || die;
    ("/usr/tmp/dods_cache#test#test_file" eq decompress_and_cache("test/test_file.Z",
						       "/usr/tmp")) || die;
    (-e "/usr/tmp/dods_cache#test#test_file") || die;
    (89 == -s "/usr/tmp/dods_cache#test_file") || die;
    print "\t decompress_and_cache passed all tests\n";

    (178 == purge_cache("/usr/tmp", 1)) || die;
    (0 == purge_cache("/usr/tmp", 0)) || die;
    print "\t purge_cache passed all tests\n";

    #unlink "/usr/tmp/dods_cache#test_file"; # get rid of the test files.
    #unlink "/usr/tmp/dods_cache#test#test_file";

    print "All tests succeeded\n";
}

1;

# $Log: DODS_Cache.pm,v $
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
# filesystem fills.   This only effects the hdf server.
#
# Revision 1.2  1997/10/09 22:19:17  jimg
# Resolved conflicts in merge of 2.14c to trunk.
#
# Revision 1.1  1997/09/30 22:16:50  jimg
# Created. Initially for use with the JPL Pathfinder server.
