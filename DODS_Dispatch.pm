
# (c) COPYRIGHT URI/MIT 1997
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
# URL. 
sub new {
    my $type = shift;
    my $self = {};  
    bless $self, $type;
    $self->initialize();
    return $self;
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

sub script {
    my $self = shift;
    my $script = shift;		# The second arg is optional

    if ($script eq "") {
	return $self->{'script'};
    } else {
	return $self->{'script'} = $script;
    }
}
    
sub command {
    my $self = shift;

    my $ext = $self->extension();
    my $cgi_dir = $self->cgi_dir();
    my $script = $self->script();
    my $filename = $self->filename();

    if ($ext eq "info") {
	# If the user wants to run the usage filter ($ext is `.html'),
	# then select that filter and send the cgi directory and API prefix
	# as the second argument (which is passed using $query).
	$server_pgm = $cgi_dir . "usage";
	$query = $cgi_dir . $script;
	$command = $server_pgm . " " . $filename . " " . "\"" . $query . "\"";
    } elsif ($ext eq "ver" || $ext eq "/version") {
	$script_rev = '$Revision: 1.2 $ ';
	$script_rev =~ s@\$([A-z]*): (.*) \$@$2@;
	$server_pgm = $cgi_dir . $script . "_dods";
	$command = $server_pgm . " -v " . $script_rev . " " . $filename;
    } elsif ($ext eq "das" || $ext eq "dds" || $ext eq "dods") {
	my $query = $self->query();
	# Otherwise, form the name of the filter program to run by catenating
	# the script name, underscore and the ext.
	$server_pgm = $cgi_dir . $script . "_" . $ext;
	$command = $server_pgm . " " . $filename . " " . "\"" . $query . "\"";
    } else {
	$command = "";
    }

    return $command;
}

my $DODS_Para1 = "The URL extension did not match any that are known by this
server. Below is a list of the five extensions that are be recognized by
all DODS servers. If you think that the server is broken (that the URL you
submitted should have worked), then please contact the\n";

my $DODS_Local_Admin = "administrator of this site at: ";
my $DODS_Support = "DODS user support coordinator at: ";

my $DODS_Para2 = "\nThe URL you sent must include one of the following five
file extensions: .das, .dds, .dods, .info or .ver.
The extensions tell the DODS server which object to return:<dl>
<dt> das  <dd> attribute object
<dt> dds  <dd> data type object
<dt> dods <dd> data object
<dt> info <dd> info object (attributes, types and other information)
<dt> ver  <dd> return the version number of the server</dl>

<p>Suggestion: If you're typing this URL into a WWW browser and
would like information about the dataset, use the `.info' extension";

# This method takes three arguments; the object,a string which names the
# script's version number and an address for mailing bug reports. If the last
# parameter is not supplied, use the maintainer address from the environment
# variables. 
sub print_error_msg {
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

1;
