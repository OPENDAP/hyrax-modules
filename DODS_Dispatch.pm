
# (c) COPYRIGHT URI/MIT 1997,1998
# Please read the full copyright statement in the file COPYRIGH.  
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

use Cwd;

sub initialize {
    my $self = shift;

    $self->{'cgi_dir'} = cwd . "/";

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
		
    $self->{'ext'} = $ext;

    # Strip the `nph-' off of the front of the dispatch program's name. Also 
    # remove the extraneous partial-pathname that HTTPD glues on to the
    # script name. 
    $script = $ENV{'SCRIPT_NAME'};
    $script =~ s@.*nph-(.*)@$1@;

    $self->{'script'} = $script;

    # Look for an ext on the script name; if one is present assume we
    # are dealing with a `secure' server (one that limits access to a
    # particular group of users). Remove the ext and set a flag
    # indicating that this sever is secure.
    if ($script =~ m@(.*)\..*@) {
	$script =~ s@(.*)\..*@$1@;	# Remove any ext if present
	$secure = 1;
    } else {
	$secure = 0;
    }

    # Look for the Accept-Encoding header. Does it exist? If so, store the
    # value. 
    $self->{'encoding'} = $ENV{'HTTP_ACCEPT_ENCODING'};

    # Look for the XDODS-Accept-Types header. If it exists, store its value.
    $self->{'accept_types'} = $ENV{'HTTP_XDODS_ACCEPT_TYPES'};

    # If this is a `secure' server, add the pathname from the document root
    # to the CGI into PATH_TRANSLATED. Because CGI 1.1 does not pass DOCUMENT
    # ROOT into the CGI we must extract it from PATH_TRANSLATED using
    # PATH_INFO. This is pretty messy stuff; if you want to see what is
    # really going on, look at the output of test-cgi.sh.
    if ($secure == 1) {
	$doc_root = $ENV{'PATH_TRANSLATED'};
	$path_info = $ENV{'PATH_INFO'};
	$doc_root =~ s@(.*)$path_info@$1@;
	$path_info =~ s@(.*)\.$ext@$1@;
	$doc_path = $ENV{'SCRIPT_NAME'};
	$doc_path =~ s@(.*)/.*@$1@;
	$filename = $doc_root . $doc_path . $path_info;
    } else {
	# Get the name of the filter to run from the data set's `ext'.
	$filename = $ENV{'PATH_TRANSLATED'};
	$filename =~ s@(.*)\.$ext@$1@;
    }

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

    my $ext = $self->extension();
    my $cgi_dir = $self->cgi_dir();
    my $cache_dir = $self->cache_dir();
    my $script = $self->script();
    my $filename = $self->filename();
    my $accept_types = $self->accept_types();

    # If the user wants to see info, version of help information, provide
    # that. Otherwise, form the name of the filter program to run by
    # catenating the script name, underscore and the ext.
    if ($ext eq "info") {
	$server_pgm = $cgi_dir . "usage";
	$full_script = $cgi_dir . $script;
	$command = $server_pgm . " " . $filename . " " . $full_script;
    } elsif ($ext eq "ver" || $ext eq "/version") {
	$script_rev = '$Revision: 1.8 $ ';
	$script_rev =~ s@\$([A-z]*): (.*) \$@$2@;
	$server_pgm = $cgi_dir . $script . "_dods";
	$command = $server_pgm . " -v " . $script_rev . " " . $filename;
    } elsif ($ext eq "help" || $ext eq "/help") {
	$self->print_help_message();
	exit(0);
    } elsif ($ext eq "das" || $ext eq "dds") {
	my $query = $self->query();
	$server_pgm = $cgi_dir . $script . "_" . $ext;
	$command = $server_pgm . " " . $filename;
	if ($query ne "") {
	    $command .= " -e " . "\"" . $query . "\"";
	}
	if ($cache_dir ne "") {
	    $command .= " -r " . "\"" . $cache_dir . "\"";
	}
	if ($accept_types ne "") {
	    $command .= " -t " . "\"" . $accept_types . "\"";
	}
    } elsif ($ext eq "dods") {
	my $query = $self->query();
	$server_pgm = $cgi_dir . $script . "_" . $ext;
	$command = $server_pgm . " " . $filename;
	if ($query ne "") {
	    $command .= " -e " . "\"" . $query . "\"";
	}
	if ($cache_dir ne "") {
	    $command .= " -r " . "\"" . $cache_dir . "\"";
	}
	if ($self->encoding() =~ m/deflate/) {
	    $command .= " -c";
	}
	if ($accept_types ne "") {
	    $command .= " -t " . "\"" . $accept_types . "\"";
	}
    } elsif ($ext eq "ascii" || $ext eq "asc") {
	my $query = $self->query();
	$server_pgm = $cgi_dir . $script . "_dods";
	$command = $server_pgm . " " . $filename;
	if ($query ne "") {
	    $command .= " -e " . "\"" . $query . "\"";
	}
	# Never compress ASCII.
	local($ascii_srvr) = $cgi_dir . "asciival";
	$command .= " | " . $ascii_srvr . " -m -- -";
    } else {
	$self->print_error_message($self->caller_revision(), 
				   $self->maintainer());
	exit(1);
    }

    return $command;
}

my $DODS_Para1 = "The URL extension did not match any that are known by this
server. Below is a list of the five extensions that are be recognized by
all DODS servers. If you think that the server is broken (that the URL you
submitted should have worked), then please contact the\n";

my $DODS_Local_Admin = "administrator of this site at: ";
my $DODS_Support = "DODS user support coordinator at: ";

my $DODS_Para2 = "\nThe URL sent to the server must include one of the
following six file extensions: .das, .dds, .dods, .info, .ver or .help.
The extensions tell the DODS server which object to return:<dl>
<dt> das  <dd> attribute object
<dt> dds  <dd> data type object
<dt> dods <dd> data object
<dt> info <dd> info object (attributes, types and other information)
<dt> ver  <dd> return the version number of the server
(dt> help <dd> help information (this text)</dl>

<p><b>Note</b>: Many DODS clients supply these extensions for you so you don't
need to append them (for example when using interfaces supplied by us or
software re-linked with a DODS client-library). Generally, you only need to
add these if you are typing a URL directly into a WWW browser.

<p><b>Suggestion</b>: If you're typing this URL into a WWW browser and
would like information about the dataset, use the `.info' extension";

# This method takes three arguments; the object, a string which names the
# script's version number and an address for mailing bug reports. If the last
# parameter is not supplied, use the maintainer address from the environment
# variables. 
# Note that this mfunc takes the script_rev and address information as
# arguments for historical resons. That information is now part of the object.
# 2/10/1998 jhrg
sub print_error_message {
    my $self = shift;
    my $script_rev = shift;
    my $address = shift;
    my $local_admin = 0;

    if ($address eq "") {
	$address = 'support@dods.gso.uri.edu';
    } else {
	$local_admin = 1;
    }

    # Note that 400 is the error code for `Bad Request'.

    print "HTTP/1.0 400 Bad Request: DODS Server filter program not found.\n";
    print "Server: ", $self->{'script'}, "/", $script_rev, "\n";
    print "\n";
    print "<h3>Error in URL</h3>\n";

    print $DODS_Para1;
    if ($local_admin == 1) {
	print $DODS_Local_Admin, $address;
    } else {
	print $DODS_Support, $address;
    }
    print "<p>\n";

    print $DODS_Para2;
}

sub print_help_message {
    my $self = shift;

    print "HTTP/1.0 200 OK\n";
    print "Server: ", $self->{'script'}, "/", $script_rev, "\n";
    print "\n";

    print "<h3>DODS Server Help</h3>\n";

    print $DODS_Para2;
}

1;
