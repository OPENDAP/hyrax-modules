
# (c) COPYRIGHT URI/MIT 1997,1998,1999
# Please read the full copyright statement in the file COPYRIGHT.
#
# Authors:
#      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
#
# Do dispatch for DODS servers. Use the `MIME type extension' of the URL to
# select the correct DODS server program. This dispatch cgi assumes that the
# DODS data server consists of four programs: *_das, *_dds, *_dods and usage,
# where `*' is the root name of the dispatch program. Each of these programs
# takes one or two arguments; a file name and possibly a query string.
#
# A Url is handled thus: 
# http://machine/cgi-bin/nph-nc/file.nc.dods?val
#                            ^^ ^^^^^^^ ^^^^ ^^^
#                            |  |       |    \
#                            |  |       \     - Constraint expression (arg 2)
#                            |  |        - selects filter (e.g., nc_*dods*)    
#                            | 	\			    		     
#      	       	       	     \ 	 - File to open (arg 1)			 
#      	       	       	      - Root name of the filter (e.g., *nc*_dods)
#
# $Log: DODS_Dispatch.pm,v $
# Revision 1.14  1999/05/21 17:18:09  jimg
# Changed quoting of various strings, esp the $query. Since the command
# arguments are now stored in a Perl list and passes to exec in that list (and
# not a single string), the command is not evaluated by the shell. Thus, quotes
# won't be removed by the shell and wind up confusing the parsers.
#
# Revision 1.13  1999/05/19 23:33:15  jimg
# Fixes for security holes. The CWD module is no longer used; this makes it
# simpler to run perl using the -T (taint) mode.
# Variables passed to executables are scanned for nasty things (shell meta
# characters).
# The commands are run straight from perl, not using an intermediate shell.
#
# Revision 1.12  1999/05/18 20:01:58  jimg
# Fixed version feature and help feature so that they work with nph-*/version,
# nph-*/version/, and nph-*/ (the latter for help).
# Fixed the help message so that it does not say `Error...' (suggested by
# the GCMD).
#
# Revision 1.11  1999/05/05 00:38:46  jimg
# Fixed the help message so that it no longer says `Error'.
# When a URL with no extension is used the help message, not the error message,
# is printed.
# Added use of the -v option to all calls to the server filter programs.
# The .ver/version extension now uses the new -V option (see DODSFilter).
#
# Revision 1.10  1999/05/04 19:47:21  jimg
# Fixed copyright statements. Removed more of the GNU classes.
#
# Revision 1.9  1999/04/29 02:37:12  jimg
# Fix the secure server stuff.
#
# Revision 1.8.4.1  1999/04/26 19:04:44  jimg
# Dan's fixes for the secure server code. The script and filename variables are
# now set correctly when data files are located in user directories (e.g.,
# ~bob/data/file.dat).
#
# Revision 1.8  1999/02/20 01:36:52  jimg
# Recognizes the XDODS-Accept-Types header (passed to the CGI using an
# environment variable). Passes along the value to the _dds and _dods filters
# using the -t option.
#
# Revision 1.7  1998/08/06 16:13:46  jimg
# Added cache dir stuff (from jeh).
#
# Revision 1.6  1998/03/17 17:20:54  jimg
# Added patch for the new ASCII filter. Use either the suffix .ascii or .asc
# to get data back in ASCII form from a DODS server.
#
# Revision 1.5  1998/02/11 22:05:59  jimg
# Added tests and an accessor function for the Accept-Encoding header (which
# CGI 1.1 passes to the cgi program using the environment variable
# HTTP_ACCEPT_ENCODING). When found with the value `deflate' the data filter
# (nc_dods, ...) is called with the -c flag which causes DODSFilter::send_data
# to try to compress the data stream using deflate (LZW from zlib 1.0.4).
# Also added a help message (activated with /help or /help).
# Fixed the error text (but it is often blocked by clients because of the http
# 400 code).
#
# Revision 1.4  1997/08/27 17:19:56  jimg
# Fixed error in -e option when requesting the DAS.
#
# Revision 1.3  1997/08/27 00:47:48  jimg
# Modified to accommodate the new DODSFilter class; added `-e' for the
# constraint expression. Hack the nph-* script to add -d and -f to $command
# to specify various weird filename/directory locations for ancillary files.
#
# Revision 1.2  1997/06/05 23:17:39  jimg
# Added to the accesser functions so that they can be used to set the field
# values in addition to reading values from the `object'.
#
# Revision 1.1  1997/06/02 21:04:35  jimg
# First version
#

package DODS_Dispatch;

sub send_error_object {
    my $msg = shift;
    my $error_obj = "\
Error {
    code = 0;
    message = \"DODS Server: $msg\";
};";
    
    # Return an error to the client.
    print(STDOUT $error_obj, "\n");
    # Print a message to the http server's error log.
    my $date = localtime;
    print(STDERR "[$date] DODS Server error: ", $msg, "\n");
}

# Security issues: All the environment variables used to get information that
# is passed to programs as arguments should be screened for hidden shell
# commands such as `cat /etc/passwd', `rm -r *', ... unless we can be
# otherwise sure that embedded shell commands will never be run. The
# environment variables used are: 
#
# QUERY_STRING: Contains the DODS CE
# PATH_INFO: Used to extract the extension from the filename which is used to
# choose the server's filter program (.das --> nc_das, etc.)
# SCRIPT_NAME: Used to build the `basename' part of the server's fileter
# program (nc --> nc_das, etc.).
# PATH_TRANSLATED: Used to get the file/dataset name.

sub initialize {
    my $self = shift;

    $self->{'cgi_dir'} = "./";

    $query = $ENV{'QUERY_STRING'};
    $query =~ tr/+/ /;		# Undo escaping by client.

    $query =~ s@\(@\\\(@g;	# escape left parentheses
    $query =~ s@\)@\\\)@g;	# escape right parentheses
    $query =~ s@\"@\\\"@g;	# escape quotes

    $self->{'query'} = $query;

    # Get the filename's ext. This tells us which filter to run. It
    # will be something like `.dods' (for data) or `.dds' (for the DDS
    # object).
    $ext = $ENV{'PATH_INFO'};
    $ext =~ s@.*\.(.*)@$1@;

    if ($ext =~ /[^A-Za-z\/]+/) {
	print(STDERR "DODS_Dispatch.pm: ext: ", $ext, "\n") if $debug >= 2;
	send_error_object("Extension contains bad characters.");
	exit(1);
    }

    $self->{'ext'} = $ext;

    # Strip the `nph-' off of the front of the dispatch program's name. Also 
    # remove the extraneous partial-pathname that HTTPD glues on to the
    # script name. 
    $script = $ENV{'SCRIPT_NAME'};
    $script =~ s@.*nph-(.*)@$1@;

    # Look for an ext on the script name; if one is present assume we
    # are dealing with a `secure' server (one that limits access to a
    # particular group of users). Remove the ext.
    if ($script =~ m@(.*)\..*@) {
	$script =~ s@(.*)\..*@$1@;	# Remove any ext if present
    }

    if ($script =~ /[^A-Za-z0-9]+/) {
	print(STDERR "DODS_Dispatch.pm: script: ", $script, "\n") 
	    if $debug >= 2;
  	send_error_object("Script name contains bad characters.");
  	exit(1);
    }
    $self->{'script'} = $script;

    # Look for the Accept-Encoding header. Does it exist? If so, store the
    # value. 
    $self->{'encoding'} = $ENV{'HTTP_ACCEPT_ENCODING'};

    # Look for the XDODS-Accept-Types header. If it exists, store its value.
    $self->{'accept_types'} = $ENV{'HTTP_XDODS_ACCEPT_TYPES'};

    $filename = $ENV{'PATH_TRANSLATED'};

    # This odd regexp excludes shell metacharacters, spaces and %-escapes
    # (e.g., %3B) from $filename which plugs a security hole that provided a
    # way to cat any file on the server's machine. E.G.:geturl
    # http://.../nph-nc/a.ver%3Bcat%20/etc/passwd%20.ver would return the
    # contents of /etc/passwd. 5/18/99 jhrg

    $filename =~ s@([^ !%&()*+?<>]*)\.$ext(.*)@$1@;
    printf(STDERR "filename: %s\n", $filename) if $debug == 2;

    $self->{'filename'} = $filename;
}

# Extract various environment variables used to pass `parameters' encoded in
# URL. The two arguments to this ctor are the current revision of the caller
# and an email adrress of the dataset/server maintainer. 
#
# Note that the $type variable is used so that DODS_Dispatch my be
# subclassed. See the perlobj man page for more information. 7/27/98 jhrg
sub new {
    my $type = shift;
    my $caller_revision = shift;
    my $maintainer = shift;

    my $self = {};  
    bless $self, $type;
    $self->initialize();

    $self->{'caller_revision'} = $caller_revision;
    $self->{'maintainer'} = $maintainer;

    return $self;
}

# Note that caller_revision and maintainer are read only fields. 2/10/1998
# jhrg
sub caller_revision {
    my $self = shift;
    return $self->{'caller_revision'};
}

sub maintainer {
    my $self = shift;
    return $self->{'maintainer'};
}

# Return the query string given with the URL.
sub query {
    my $self = shift;
    my $query = shift;		# The second arg is optional

    if ($query eq "") {
	return $self->{'query'};
    } else {
	return $self->{'query'} = $query;
    }
}    

# If the second argument is given, use it to set the filename member.
sub filename {
    my $self = shift;
    my $filename = shift;	# The second arg is optional

    if ($filename eq "") {
	return $self->{'filename'};
    } else {
	return $self->{'filename'} = $filename;
    }
}

sub extension {
    my $self = shift;
    my $extension = shift;	# The second arg is optional

    if ($extension eq "") {
	return $self->{'ext'};
    } else {
	return $self->{'ext'} = $extension;
    }
}

sub cgi_dir {
    my $self = shift;
    my $cgi_dir = shift;	# The second arg is optional

    if ($cgi_dir eq "") {
	return $self->{'cgi_dir'};
    } else {
	return $self->{'cgi_dir'} = $cgi_dir;
    }
}

sub cache_dir {
    my $self = shift;
    my $cache_dir = shift;	# The second arg is optional

    if ($cache_dir eq "") {
	return $self->{'cache_dir'};
    } else {
	return $self->{'cache_dir'} = $cache_dir;
    }
}

sub script {
    my $self = shift;
    my $script = shift;		# The second arg is optional

    if ($script eq "") {
	return $self->{'script'};
    } else {
	return $self->{'script'} = $script;
    }
}
    
# Unlike the other accessor functions you *cannot* set the value encoding. It
# can only be set by the request header.

sub encoding {
    my $self = shift;

    return $self->{'encoding'};
}

sub accept_types {
    my $self = shift;
    
    return $self->{'accept_types'};
}

sub command {
    my $self = shift;

    # If the user wants to see info, version of help information, provide
    # that. Otherwise, form the name of the filter program to run by
    # catenating the script name, underscore and the ext.
    if ($ext eq "info") {
	$server_pgm = $self->cgi_dir() . "usage";
	# usage does not support the version flag like the other filter
	# programs. 5/19/99 jhrg
	# $server_pgm .= " -v " . $self->{'caller_revision'} . " ";
	$full_script = $self->cgi_dir() . $self->script();
	@command = ($server_pgm, $self->filename(), $full_script);
    } elsif ($ext eq "ver" || $ext eq "/version" || $ext eq "/version/") {
	$server_pgm = $self->cgi_dir() . $self->script() . "_dods";
	@command = ($server_pgm, "-v", $self->caller_revision(), "-V",
	            $self->filename());
    } elsif ($ext eq "help" || $ext eq "/help" || $ext eq "" || $ext eq "/") {
	$self->print_help_message();
	exit(0);
    } elsif ($ext eq "das" || $ext eq "dds") {
	my $query = $self->query();
	my $cache_dir = $self->cache_dir();
	my $accept_types = $self->accept_types();
	$server_pgm = $self->cgi_dir() . $self->script() . "_" . $ext;
	@command = ($server_pgm, "-v", $self->{'caller_revision'}, 
		    $self->filename());
	if ($query ne "") {
	    @command = (@command, "-e", $query);
	}
	if ($cache_dir ne "") {
	    @command = (@command, "-r", $cache_dir);
	}
	if ($accept_types ne "") {
	    @command = (@command, "-t", $accept_types);
	}
    } elsif ($ext eq "dods") {
	my $query = $self->query();
	my $cache_dir = $self->cache_dir();
	my $accept_types = $self->accept_types();
	$server_pgm = $self->cgi_dir() . $self->script() . "_" . $ext;
	@command = ($server_pgm, "-v", $self->{'caller_revision'}, 
		    $self->filename());
	if ($query ne "") {
	    @command = (@command, "-e", $query);
	}
	if ($cache_dir ne "") {
	    @command = (@command, "-r", $cache_dir);
	}
	if ($self->encoding() =~ /deflate/) {
	    @command = (@command, "-c");
	}
	if ($accept_types ne "") {
	    @command = (@command, "-t", $accept_types);
	}
    } elsif ($ext eq "ascii" || $ext eq "asc") {
	my $query = $self->query();
	$server_pgm = $self->cgi_dir() . $self->script() . "_dods";
	@command = ($server_pgm, "-v", $self->{'caller_revision'}, 
		    $self->filename());
	if ($query ne "") {
	    @command = (@command, "-e", $query);
	}
	# Never compress ASCII.
	local($ascii_srvr) = $cgi_dir . "asciival";
	@command = (@command, "|", $ascii_srvr, " -m -- -");
    } else {
	$self->print_error_message();
	exit(1);
    }

    print(STDERR "DODS server command: ", @command, "\n") if $debug;
    return @command;
}

my $DODS_Para1 = "The URL extension did not match any that are known by this
server. Below is a list of the five extensions that are be recognized by
all DODS servers. If you think that the server is broken (that the URL you
submitted should have worked), then please contact the\n";

my $DODS_Local_Admin = "administrator of this site at: ";
my $DODS_Support = "DODS user support coordinator at: ";

my $DODS_Para2 = "To access most of the features of this DODS server, append
one of the following a five suffixes to a URL: .das, .dds, .dods., .info,
.ver or .help. Using these suffixes, you can ask this server for:<dl>
<dt> das  <dd> attribute object
<dt> dds  <dd> data type object
<dt> dods <dd> data object
<dt> info <dd> info object (attributes, types and other information)
<dt> ver  <dd> return the version number of the server
<dt> help <dd> help information (this text)</dl>
</dl>
For example, to request the DAS object from the FNOC1 dataset at URI/GSO (a
test dataset) you would appand `.das' to the URL: http://dods.gso.uri.edu/gi-bin/nph-nc/data/fnoc1.nc.das.

<p><b>Note</b>: Many DODS clients supply these extensions for you so you don't
need to append them (for example when using interfaces supplied by us or
software re-linked with a DODS client-library). Generally, you only need to
add these if you are typing a URL directly into a WWW browser.

<p><b>Note</b>: If you would like version information for this server but
don't know a specific data file or data set name, use `/version' for the
filename. For example: http://dods.gso.uri.edu/cgi-bin/nph-nc/version will
return the version number for the netCDF server used in the first example. 

<p><b>Suggestion</b>: If you're typing this URL into a WWW browser and
would like information about the dataset, use the `.info' extension\n";

# This method takes three arguments; the object, a string which names the
# script's version number and an address for mailing bug reports. If the last
# parameter is not supplied, use the maintainer address from the environment
# variables. 
#
# Note that this mfunc takes the script_rev and address information as
# arguments for historical resons. That information is now part of the object.
# 2/10/1998 jhrg
#
# Futher changed the dispatch script. The caller_revision and maintainer
# fields are used explicitly and the args are ignored. 5/4/99 jhrg

sub print_error_message {
    my $self = shift;
    my $local_admin = 0;

    if ($self->{'maintainer'} ne "support\@unidata.ucar.edu") {
	$local_admin = 1;
    }

    # Note that 400 is the error code for `Bad Request'.

    print "HTTP/1.0 400 DODS server filter program not found.\n";
    print "XDODS-Server: ", $self->{'script'}, "/", 
          $self->{'caller_revision'}, "\n";
    print "\n";
    print "<h3>Error in URL</h3>\n";

    print $DODS_Para1;
    if ($local_admin == 1) {
	print $DODS_Local_Admin, $self->{'maintainer'};
    } else {
	print $DODS_Support, $self->{'maintainer'};
    }
    print "<p>\n";

    print $DODS_Para2;
}

sub print_help_message {
    my $self = shift;

    print "HTTP/1.0 200 OK\n";
    print "XDODS-Server: ", $self->{'script'}, "/", $self->{'caller_revision'}, "\n";
    print "\n";

    print "<h3>DODS Server Help</h3>\n";

    print $DODS_Para2;
}

1;
