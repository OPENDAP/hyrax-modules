
# (c) COPYRIGHT URI/MIT 1997
# Please read the full copyright statement in the file COPYRIGH.  
#
# Authors:
#      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
#
# Clean the cache directory. Removed oldest files until $cache contains less
# than cache_max_size bytes.
#
# Example use:
# $cache_size = purge_cache(".", 300000, "^[A-z].*\.cc.*");
# print "cache now occupies $cache_size bytes\n";
#
# $Log: DODS_Cache.pm,v $
# Revision 1.2  1997/10/09 22:19:17  jimg
# Resolved conflicts in merge of 2.14c to trunk.
#
# Revision 1.1  1997/09/30 22:16:50  jimg
# Created. Initially for use with the JPL Pathfinder server.
#

package DODS_Cache;

sub reverse_by_times {
    ($times{$a} <=> $times{$b}) * -1;
}

sub by_times {
    $times{$a} <=> $times{$b};
}

# Manage the cache directory. DODS servers which work with compressed files
# often decompress the files using a perl script and then pass the name of
# the decompressed file onto one or more filter functions. This subroutine
# helps manage the cache of decompressed files so that files can be used
# several times without repeated decompression while not filling up the
# host's disk. Files are removed from the cache directory using oldest-first
# strategy. 
#
# The arguments to this subroutine are: 
# CACHE: the directory that holds the cached files
# CACHE_MAX_SIZE: prune files from the cache until the size in bytes of the
# cache is less than this value.
# CACHED_NAMES_REGEX: only consider removing files whose names match the
# given regex.

sub main::purge_cache {
    local($cache, $cache_max_size, $cached_names_regex) = @_;

    local($cache_size) = 0;
    
    # Read the cache directory contents.
    opendir(CACHE, $cache) || die "Could not open the cache directory!\n";
    local(@files) = grep(/$cached_names_regex/, readdir(CACHE));
    closedir(CACHE);

    local(%links, %sizes, %times, $nlink, $size, $atime);
    foreach $file (@files) {
	# $d is a dummy placeholder
	($d, $d, $d, $nlink, $d, $d, $d, $size, $atime) = stat($file);
	$links{$file} = $nlink;
	$sizes{$file} = $size;
	$times{$file} = $atime;
	$cache_size += $size;	# $cache size init'd at top.
	# print "$file: $links{$file}, $sizes{$file}, $times{$file}\n";
    }

    # print STDERR "Spool size: $cache_size\n";
    # print STDERR "Spool max size: $cache_max_size\n";

    # Removed oldest files first. Continue removing files until cache size
    # falls below max size. 
    local(@files_sorted_by_times) = sort(by_times keys(%times));

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

1;
