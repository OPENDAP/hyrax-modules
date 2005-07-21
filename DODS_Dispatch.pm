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

# (c) COPYRIGHT URI/MIT 1997,1998,1999,2000
# Please read the full copyright statement in the file COPYRIGHT_URI
#
# Authors:
#      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

# Do dispatch for DODS servers. Use the `MIME type extension' of the URL to
# select the correct DODS server program. This dispatch cgi assumes that the
# DODS data server consists of four programs: *_das, *_dds, *_dods and usage,
# where `*' is the root name of the dispatch program. Each of these programs
# takes one or two arguments; a file name and possibly a query string.
#
# We've added to this set of programs. www_int and asciival are now required.
# 5/31/2001 jhrg
#
# A Url is handled thus:
# http://machine/cgi-bin/nph-dods/file.nc.dods?val
#                                 ^^^^^^^ ^^^^ ^^^
#                                 |       |    \
#                                 |       \     - Constraint expression
#                                 |        - selects filter
#                                 \
#      	       	       	           - File to open (arg 1)
#
# Shell meta characters: &;`'\"|*?~<>^()[]{}$\n\r

package DODS_Dispatch;

# No symbols are exported.

use Env;
use read_config;
use dods_logging;
use DODS_Cache;

my $debug = 2;
my $test  = 0;

# Error message for bad extensions.
my $unknown_ext = "The URL extension did not match any that are known by this
server. Below is a list of the five extensions that are be recognized by
all DODS servers. If you think that the server is broken (that the URL you
submitted should have worked), then please contact the\n";

# Bad file/dataset types.
my $unknown_p1 = "This DODS server does not know how to serve the dataset\n`";
my $unknown_p2 = ".'
It maybe that the server has not been configured for this type of dataset.
Please double check the URL for errors and, if you think that the URL is
correct, please contact the ";

# Bad characters in the URL.
my $bad_chars = "Bad characters in URL. If you think this URL is correct, 
please contact the ";


# Test if variables are tainted.
# From Programming Perl, p.258. 12/11/2000 jhrg
sub is_tainted {
    not eval {
        join( "", @_ ), kill 0;    # Perl warns about join; don't listen to it.
        1;
    };
}

sub is_directory {
    return -d $_[0];
}

sub is_file {
    return -f $_[0];
}

# Given a time string that's nominally RFC822/1023 compliant, return the
# matching Unix time. Assume that the time string is in GMT.
sub rfc822_to_time {
    use Time::Local;
    my $time_string = shift;
    my ( $dummy, $mday, $mon_name, $year, $hour, $min, $sec );
    my %mon = (
                Jan => 0,
                Feb => 1,
                Mar => 2,
                Apr => 3,
                May => 4,
                Jun => 5,
                Jul => 6,
                Aug => 7,
                Sep => 8,
                Oct => 9,
                Nov => 10,
                Dec => 11
    );

    print( DBG_LOG "In RFC822_to_time: $time_string.\n" ) if $debug >= 1;

    # Look for two common date strings, otherwise punt. 12/11/2001 jhrg
    if ( $time_string =~ /[A-z]+ [A-z]+ [0-3][0-9] .*/ ) {
        ( $dummy, $mon_name, $mday, $hour, $min, $sec, $year ) = split /[:, ]+/,
          $time_string;
    } elsif ( $time_string =~ /[A-z]+,* [0-3][0-9] [A-z]+ .*/ ) {
        ( $dummy, $mday, $mon_name, $year, $hour, $min, $sec ) = split /[:, ]+/,
          $time_string;
    } else {
        print( DBG_LOG "unrecognizable time string: ", $time_string, "\n" )
          if $debug > 0;
        return -1;
    }

    return timegm( $sec, $min, $hour, $mday, $mon{$mon_name}, $year );
}

# Security issues: All the environment variables used to get information
# passed to programs as arguments should be screened for hidden shell
# commands such as `cat /etc/passwd', `rm -r *', ... unless we can be
# otherwise sure that embedded shell commands will never be run. The
# environment variables used are:
#
# HTTP_HOST, SERVER_NAME
# SERVER_ADMIN
# QUERY_STRING: Contains the DODS CE
# PATH_INFO: Used to extract the extension from the filename which is used to
# pass the response (object) type to the filter program. PATH_INFO is
# the path-part of the URL which follows the CGI up to a '?' character.
# SCRIPT_NAME: Used to build the request_uri field's value. Used by asciival, et c.
# PATH_TRANSLATED: Used to get the file/dataset name. PATH_TRANSLATED is 
# the result of performing a logical to physical mapping on PATH_INFO. If
# data_root is set in dap_server.rc, that is used along with PATH_INFO 
# instead of PATH_TRANSLATED.
# HTTP_ACCEPT_ENCODING: Used to indicate that the client can understand
# compressed responses.
# HTTP_IF_MODIFIED_SINCE: Used with conditional requests.

sub initialize {
    my $self = shift;
    my $data;    # temp used to hold data from %ENV

    open( DBG_LOG, ">> /usr/local/tmp/dbg_log" ) if $debug;
    
    print( DBG_LOG "------------------------------------------------\n" ) if $debug;
    print( DBG_LOG "DODS Server debug log: ", scalar localtime, "\n" ) if $debug;

    # Read the configuration file.
    my ( $timeout, $cache_dir, $cache_size, $maintainer, $curl, $data_root, 
         @exclude ) = get_params( $self->{config_file} );
    $self->timeout($timeout);
    $self->cache_dir($cache_dir);
    $self->cache_size($cache_size);
    $self->maintainer($maintainer);
    # If data_root is not set, then use PATH_TRANSLATED as with 3.4 and
    # earlier. jhrg 5/27/05
    if ($data_root ne "") {
        $self->data_path("$data_root$ENV{PATH_INFO}"); 
    }
    else {
        $self->data_path("$ENV{PATH_TRANSLATED}");
    }
    $self->curl($curl);
    
    $self->exclude(@exclude);

    print( DBG_LOG "Timeout: ",    $self->timeout(),    "\n" ) if $debug > 1;
    print( DBG_LOG "Cache Dir: ",  $self->cache_dir(),  "\n" ) if $debug > 1;
    print( DBG_LOG "Cache Size: ", $self->cache_size(), "\n" ) if $debug > 1;
    print( DBG_LOG "Maintainer: ", $self->maintainer(), "\n" ) if $debug > 1;

    print( DBG_LOG "data path: ", $self->data_path(), "\n" ) if $debug > 1;

    print( DBG_LOG "Exclude: ",    $self->exclude(),    "\n" ) if $debug > 1;

    # Process values read from the CGI 1.1 environment variables.

    $self->{cgi_dir} = "./";

    # it seems apache 2 does not use this; version 1 did. It may be that
    # still other daemons put this information both here and in SERVER_NAME.
    # We test for the port here and then again in SERVER_NAME (just below).
    # The information in SERVER_PORT takes precedence over SERVER_NAME.
    # 06/04/03 jhrg
    $data = $ENV{SERVER_PORT};

    # Sanitize.
    if ( $data =~ /^[~0-9]*([0-9]*)[~0-9]*$/ ) {
        $self->{server_port} = $1;
    } else {
        print( DBG_LOG "Error:SERVER_PORT '$ENV{SERVER_PORT}'\n" );
        $self->print_dods_error(
"Bad characters in the SERVER_PORT header. This may be a problem with
your client or with the OPeNDAP server software. If you think the probelm
is not in your client, please contact the ", 0
        );
        exit 1;
    }

    # The HOST header may not be in the http request object, but if it is use
    # it. If the host is known by an IP number and not a name that number may
    # be in the HOST header. Patch suggested by Jason Thaxter
    # <thaxter@gomoos.org>, see bug #336. 12/27/2001 jhrg
    $data = $ENV{HTTP_HOST} || $ENV{SERVER_NAME};

    # Sanitize.
    if ( $data =~ /^([-\@\w.]*):?([0-9]*)$/ ) {
        $self->{server_name} = $1;    # $data now untainted
        if ( $self->{server_port} eq "" && $2 ne "" ) {
            $self->{server_port} = $2;
        }
    } else {
        $data = $ENV{HTTP_HOST} || $ENV{SERVER_NAME};
        print( DBG_LOG "Error:HTTP_HOST or SERVER_NAME '$data'\n" );
        $self->print_dods_error(
"Bad characters in the HOST or SERVER_NAME header. This may be a problem with
your client or with the OPeNDAP server software. If you think the probelm
is not in your client, please contact the ", 0
        );
        exit 1;
    }
    print( DBG_LOG "server port: ", $self->{server_port}, "\n" ) if $debug > 1;
    print( DBG_LOG "server name: ", $self->{server_name}, "\n" ) if $debug > 1;

    # This is a potential security hole! Our variable names allow a great
    # many characters that should never be fed into a CGI. Make sure that the
    # query string is passed to executable programs that 1) never call sh and
    # that 2) it's used only with the list form of exec (which does not
    # interpret shell meta characters.
    $data = $ENV{QUERY_STRING};
    if ( $data =~ /^(.*)$/ ) {
        $self->{query} = $1;    # $data now untainted
    } else {
        print( DBG_LOG "Error:QUERY_SRING '$ENV{QUERY_STRING}'\n" );
        $self->print_dods_error( $bad_chars, 0 );
        exit 1;
    }
    $self->{query} =~ tr/+/ /;    # Undo escaping by client.
    print( DBG_LOG "query: ", $self->{query}, "\n" ) if $debug > 1;

    # Get the filename's ext. This tells us which filter to run. It
    # will be something like `.dods' (for data) or `.dds' (for the DDS
    # object).
    $ext = $ENV{PATH_INFO};
    print( DBG_LOG "PATH_INFO: ", $ext, "\n" ) if $debug > 1;

    # old comment (04/28/03 jhrg):
    # Using `s' does not untaint $ext, but using a pattern match followed
    # substring assignment does (see Programming Perl, p.358.). $ext needs to
    # be sanitized because that is used further down to sanitize $filename
    # which is passed to system() under some conditions. 12/11/2000 jhrg
    # System is no longer used (it was used to handle decompression). So,
    # it's no longer true that $filename needs to be sanitized. But, it can't
    # hurt... Additionally, in DODS_Cache.pm where system() was used  we
    # check for shell meta characters in $filename, rejecting any that are
    # found. 10/21/02 jhrg

    if ( $ext =~ /^.*(help|version|stats)(\/?)$/ ) {
        $ext = $1;
    }

    # Special case URLs for directories. 1/3/2001 jhrg
    # Use PATH_TRANSLATED for the directory test. 7/13/2001 jhrg
    elsif ( is_directory( $self->data_path() ) ) {
        $ext = "/";
    } elsif ( $ext =~ /^.*\.(das|dds|dods|ascii|asc|version|ver|info|html)$/ ) {
        $ext = $1;
    } else {
        print( DBG_LOG "DODS_Dispatch.pm: ext: ", $ext, "\n" );

        # I spiffed up this error message as per Dan's bug report #680.
        $self->print_dods_error(

"Bad characters in the URL's suffix. The server expects the filename part of
the URL to end in one of das, dds, dods, ascii, info, html or version. These
suffixes select different responses from the server. To ask the server for a
directory listing, end the URL with a slash; See the DODS/OPeNDAP User Guide
or ask the server for help (append '/help' or '.help' to any URL) to get more
information. If you think this is a server problem please contanct the\n"
        );

        exit(1);
    }

    $self->{ext} = $ext;
    print( DBG_LOG "ext: ", $self->{ext}, "\n" ) if $debug > 1;

    # REQUEST_URI is a convenience supported by apache but not Netscape's
    # FastTrack server. See bug 111. 4/30/2001 jhrg
    # my $request = $ENV{REQUEST_URI};
    # Note that we must differentiate between URLs for directories and those
    # for DODS responses. 05/07/03 jhrg
    my $request = $ENV{SCRIPT_NAME} . $ENV{PATH_INFO};
    if ( $ENV{QUERY_STRING} ne "" ) {
        $request .= "?" . $ENV{QUERY_STRING};
    }

    if ( $self->{ext} eq "/" ) {
        if ( $request !~ m@^.*$ext$@ ) {
            $request .= $ext;
            print( DBG_LOG "Hacked request ext: ", $request, "\n" )
              if $debug > 1;
        }
        $request =~ m@(.*)$self->{ext}@;
        $self->{request_uri} = $1    # Assignment must be right after match.
    } else {
        $request =~ m@(.*)\.$self->{ext}@;
        $self->{request_uri} = $1;
    }
    print( DBG_LOG "Request URI: $self->{request_uri}\n" ) if $debug > 1;

    # Now we can set the full_uri.
    $self->{full_uri} =
      "http://" . $self->server_name() . $self->port() . $self->request_uri();

    # Now extract the filename part of the path.
    my $path_info = $ENV{PATH_INFO};
    print( DBG_LOG "Second PATH_INFO access: ", $ENV{PATH_INFO}, "\n" )
      if $debug > 1;

    # Sanitize.
    if ( $ext eq "/" ) {

        # I removed a '\.' in the patern below to get this to work with
        # directories. The original pattern was @(.*)\.$ext@ 10/22/02 jhrg
        # However, *that* doesn't work with other accesses and we *have* to
        # use pattern match and assignment to sanitze path_info. 01/28/03 jhrg
        if ( $path_info !~ m@^.*$ext$@ ) {
            $path_info .= $ext;
            print( DBG_LOG "Hacked ext: ", $path_info, "\n" ) if $debug > 1;
        }
        $path_info =~ m@(.*)$ext@;
        $path_info = $1;
        print( DBG_LOG "path_info fraction (re)assigned to the variable: ",
               $path_info, "\n" )
          if $debug > 1;
    } else {
        $path_info =~ m@(.*)\.$ext@;
        $path_info = $1;
        print( DBG_LOG "path_info fraction (re)assigned to the variable: ",
               $path_info, "\n" )
          if $debug > 1;
    }
    $self->{path_info} = $path_info;

    print( DBG_LOG "path_info: ", $self->{path_info}, "\n" ) if $debug > 1;

    # Figure out which type of handler to use when processing this request.
    # The config_file field is set in new(). Note that we only use the
    # handlers to generate the DAP objects and ver and info responses;
    # everything else is passed off to a helper or taken care of by this
    # script. However, we ask for the handler for all of the extensions to
    # make sure that the server (via dods.rc) is configured for the
    # particular type of URL. If we don't do that then an errant request for
    # .html, for example, will loop forever (since it's a subordinate request
    # that accesses the dataset and that's what fails). 9/19/2001 jhrg
    #
    # Slight modification: If the handler is null ("") and the extension is a
    # slash ("/"), that's OK. See Bug 334. 12/27/2001 jhrg

    $self->{handler} = handler_name( $self->{path_info}, $self->{config_file} );
    if (    $self->{ext} ne "/"
         && $self->{ext} ne "stats"
         && $self->{ext} ne "version"
         && $self->{ext} ne "help"
         && $self->{handler} eq "" )
    {
        $self->print_dods_error( "${unknown_p1}${path_info}${unknown_p2}", 0 );
        exit(1);
    }

    print DBG_LOG "Server type: $self->{handler}\n" if $debug > 1;

    # Look for the Accept-Encoding header. Does it exist? If so, store the
    # value.
    $data = $ENV{HTTP_ACCEPT_ENCODING};
    if ( $data =~ /^.*(deflate).*$/ ) {
        $self->{encoding} = "deflate";    # $data now untainted
    } else {
        $self->{encoding} = "";
    }

    # Look for the If-Modified-Since header. Does it exist? If so, get the
    # date and convert it to Unix time.
    if ( $ENV{HTTP_IF_MODIFIED_SINCE} ne "" ) {
        $data = $ENV{HTTP_IF_MODIFIED_SINCE};
        if ( $data =~ /^([-\w0-9\s\t,;:.=]+)$/ ) {
            $self->{if_modified_since} = rfc822_to_time($1);
        } else {
            print( DBG_LOG
                   "Error:IF_MODIFIED_SINCE '$ENV{HTTP_IF_MODIFIED_SINCE}'\n" );
            $self->print_dods_error(
"Bad characters in the IF_MODIFIED_SINCE header. This may be a problem with
your client or with the OPeNDAP server software. If you think the probelm
is not in your client, please contact the ", 0
            );
            exit 1;
        }
    } else {
        $self->{if_modified_since} = -1;
    }

    print( DBG_LOG "if modified since value: ",
           $self->{if_modified_since}, "\n" )
      if $debug > 1;

    print( DBG_LOG "data path: ", $self->data_path(), "\n" )
      if $debug > 1;

    # Here's where we need to set $filename so that it's something that
    # DODS_Cache can be hacked to deal with. If $filename is set to
    # $PATH_INFO, we should be all set. We process a DODSter URL in much the
    # same way a compressed local file is processed (see nph-dods.in).
    # 10/22/02 jhrg
    if ( $self->{handler} =~ m@^.*/jg_handler$@ || is_dodster( $ENV{PATH_INFO} ) ) {
        $filename = $ENV{PATH_INFO};

        # For both DODSter and JGOFS URLs, remove PATH_INFO's leading slash.
        if ( $filename =~ m@/(.*)@ ) {
            $filename = $1;
        }
    } else {
        $filename = $self->data_path();
        if ( $filename =~ m@(.*)@ ) {
            $filename = $1;
        }
    }

    print DBG_LOG "filename(1): $filename\n" if $debug > 1;

    # Simpler regex. 12/11/2000 jhrg
    if (    $self->{ext} eq "help"
         || $self->{ext} eq "version"
         || $self->{ext} eq "stats" )
    {
        $filename = "";
    }

    # Added `:' to support DODSter. For that case, $filename will be a URL.
    # 10/22/02 jhrg
    elsif ( $filename =~ /^([-\/.\w:]+)\.$self->{ext}.*$/ )
    {    # match - / . and words
        $filename = $1;
    }

    # This makes directory URLs that end in `?M=A, et c., work by separating
    # the pseudo-query part from the `filename' part. 12/11/2001 jhrg
    elsif ( $self->{ext} eq "/" && $filename =~ /^([-\/.\w]+).*$/ ) {
        $filename = $1;
    } else {
        printf( DBG_LOG "filename: %s\n", $filename ) if $debug > 1;
        $self->print_dods_error( $bad_chars, 0 );
        exit 1;
    }

    printf( DBG_LOG "filename(2): %s\n", $filename ) if $debug > 1;

    $self->{filename} = $filename;
}

# Extract various environment variables used to pass `parameters' encoded in
# URL. The two arguments to this ctor are the current revision of the caller
# and an email address of the dataset/server maintainer.
#
# Note that the $type variable is used so that DODS_Dispatch my be
# sub-classed. See the perlobj man page for more information. 7/27/98 jhrg
#
# Added @exclude to the list of ctor params. This is a list of `handler
# names' (see the dods.rc file) that have regular expressions which should
# NOT be rerouted through the DODS server's HTML form generator. Often this
# is the case because their regexes are something like `.*'. 5/9/2001 jhrg
# The 'exclude' stuff is now handled by a setting in the dods.rc file.
# 11/19/03 jhrg
#
# At some point a fourth param was added so that it would be possible to pass
# into this object the name of the configuration file. 10/21/02 jhrg
# Now the dods.rc handles the parameters and so the other two arguments went
# away. 11/19/03 jhrg
sub new {
    my $type               = shift;
    my $caller_revision    = shift;
    my $server_config_file = shift;

    my $self = {};
    bless $self, $type;

    $self->{caller_revision} = $caller_revision;
    $self->{config_file}     = $server_config_file;

    $self->initialize();

    return $self;
}

sub caller_revision {
    my $self = shift;
    return $self->{caller_revision};
}

sub path_info {
    my $self = shift;
    return $self->{path_info};
}

sub server_port {
    my $self = shift;
    return $self->{server_port};
}

# A smart version of `server_port'. Curl barfs on urls that have the colon
# without any port number. 05/07/03 jhrg
sub port {
    my $self = shift;
    if ( $self->{server_port} == 80 || $self->{server_port} == "" ) {
        return "";
    } else {
        return ":$self->{server_port}";
    }
}

sub server_name {
    my $self = shift;
    return $self->{server_name};
}

# The request_uri is the part of the URI after the protocol, machine and
# port and goes upto, but not include, the DODS request extension.
# Example: http://dods.org/dods-3.4/nph-dods/data/dataset.hdf.dods?x,y,z
# request_uri: /dods-3.4/nph-dods/data/dataset.hdf
sub request_uri {
    my $self = shift;
    return $self->{request_uri};
}

# The full_uri is the whole URI, starting with http:// and continuing until,
# but not including, the DODS request extension.
# Example: http://dods.org/dods-3.4/nph-dods/data/dataset.hdf.dods?x,y,z
# full_uri: http://dods.org/dods-3.4/nph-dods/data/dataset.hdf
sub full_uri {
    my $self = shift;
    return $self->{full_uri};
}

sub maintainer {
    my $self  = shift;
    my $value = shift;    # Optional, use to set.

    if ( $value eq "" ) {
        return $self->{maintainer};
    } else {
        return $self->{maintainer} = $value;
    }
}

sub exclude {
    my $self   = shift;
    my $values = @_;

    if ( $#value == 0 ) {
        return $self->{exclude};
    } else {
        return $self->{exclude} = $values;
    }
}

# Return the query string given with the URL.
sub query {
    my $self  = shift;
    my $query = shift;    # The second arg is optional

    if ( $query eq "" ) {
        return $self->{query};
    } else {
        return $self->{query} = $query;
    }
}

# If the second argument is given, use it to set the filename member.
sub filename {
    my $self     = shift;
    my $filename = shift;    # The second arg is optional

    if ( $filename eq "" ) {
        return $self->{filename};
    } else {
        return $self->{filename} = $filename;
    }
}

sub ext {
    my $self      = shift;
    my $extension = shift;    # The second arg is optional

    if ( $extension eq "" ) {
        return $self->{ext};
    } else {
        return $self->{ext} = $extension;
    }
}

sub cgi_dir {
    my $self    = shift;
    my $cgi_dir = shift;      # The second arg is optional

    if ( $cgi_dir eq "" ) {
        return $self->{cgi_dir};
    } else {
        return $self->{cgi_dir} = $cgi_dir;
    }
}

sub cache_dir {
    my $self      = shift;
    my $cache_dir = shift;    # The second arg is optional

    if ( $cache_dir eq "" ) {
        return $self->{cache_dir};
    } else {
        return $self->{cache_dir} = $cache_dir;
    }
}

sub cache_size {
    my $self       = shift;
    my $cache_size = shift;    # The second arg is optional

    if ( $cache_size == 0 ) {
        return $self->{cache_size};
    } else {
        return $self->{cache_size} = $cache_size;
    }
}

sub timeout {
    my $self    = shift;
    my $timeout = shift;       # The second arg is optional

    if ( $timeout eq "" ) {
        return $self->{timeout};
    } else {
        return $self->{timeout} = $timeout;
    }
}

sub curl {
    my $self = shift;
    my $curl = shift;          # The second arg is optional

    if ( $curl eq "" ) {
        return $self->{curl};
    } else {
        return $self->{curl} = $curl;
    }
}

sub access_log {
    my $self       = shift;
    my $access_log = shift;    # The second arg is optional

    if ( $access_log eq "" ) {
        return $self->{access_log};
    } else {
        return $self->{access_log} = $access_log;
    }
}

sub error_log {
    my $self      = shift;
    my $error_log = shift;     # The second arg is optional

    if ( $error_log eq "" ) {
        return $self->{error_log};
    } else {
        return $self->{error_log} = $error_log;
    }
}

sub machine_names {
    my $self          = shift;
    my $machine_names = shift;    # The second arg is optional

    if ( $machine_names eq "" ) {
        return $self->{machine_names};
    } else {
        return $self->{machine_names} = $machine_names;
    }
}

sub is_stat_on {
    my $self  = shift;
    my $value = shift;

    if ( $value eq "" ) {
        return $self->{is_stat_on};
    } else {
        return $self->{is_stat_on} = $value;
    }
}

# This returns the full path to the handler. The set of known handlers
# is read from the dods.rc file during initialization. 
sub handler {
    my $self   = shift;
    my $handler = shift;    # The second arg is optional

    if ( $handler eq "" ) {
        return $self->{handler};
    } else {
        return $self->{handler} = $handler;
    }
}

# This returns the directory where the server binaries are stored. From the 
# configuration file (dods.rc).
sub sbin_dir {
    my $self   = shift;
    my $sbin_dir = shift;    # The second arg is optional

    if ( $sbin_dir eq "" ) {
        return $self->{sbin_dir};
    } else {
        return $self->{sbin_dir} = $sbin_dir;
    }
}

sub data_path {
    my $self   = shift;
    my $data_path = shift;    # The second arg is optional

    if ( $data_path eq "" ) {
        return $self->{data_path};
    } else {
        return $self->{data_path} = $data_path;
    }
}

# Unlike the other access or functions you *cannot* set the value encoding. It
# can only be set by the request header. The same is true for
# if_modified_since.

sub encoding {
    my $self = shift;

    return $self->{encoding};
}

sub if_modified_since {
    my $self = shift;

    return $self->{if_modified_since};
}

# Private. Get the remote thing. The param $url should be scanned for shell
# meta-characters.
sub get_url {
    my $self = shift;
    my $url  = shift;

    my $transfer = $self->curl() . " --silent " . $url . " |";
    my $buf;
    print( DBG_LOG "About to run curl: $transfer\n" ) if $debug > 1;

    # Use the HTML error message format since this is only used via a web
    # browser, never a client built with our library. 11/21/03 jhrg
    open CURL, $transfer
      or print_error_message(
        $self, "Could not transfer $url: \n\
Unable to open the transfer utility (curl).\n", 0 );
    print( DBG_LOG "Back from curl\n" ) if $debug > 1;
    my $offset = 0;
    my $bytes;
    while ( $bytes = read CURL, $buf, 20, $offset ) {
        $offset += $bytes;
    }

    close CURL;

    return $buf;
}

sub command {
    my $self = shift;

    # If the user wants to see info, version or help information, provide
    # that. Otherwise, run the handler. Pass the response type into the 
    # handler using the -o option.
    if ( $self->ext() eq "info" ) {

        # I modified this so that the caller revision and cache directory
        # information is passed to usage so that it can pass it on to the
        # filter programs. Passing the cache dir info addresses bug #453
        # where the HDF server was writing its cache files to the data
        # directory (because that's the default). 6/5/2002 jhrg
        $server_pgm = $self->sbin_dir() . "usage";
        $options    = "'-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir() . "'";
        }

        @command = ( $server_pgm, $options, $self->filename(), $self->handler() );
    } elsif ( $self->ext() eq "ver" || $self->ext() eq "version" ) {

        # if there's no filename assume `.../nph-dods/version/'. 6/8/2001 jhrg
        if ( $self->filename() eq "" ) {
            $self->send_dods_version();
            exit(0);
        } else {
            $server_pgm = $self->handler();
            @command = (
                         $self->handler(),
			 "-v", $self->caller_revision(),
			 "-o", "Version",
			 "-u", $self->full_uri(),
                         $self->filename()
            );
        }
    } elsif ( $self->ext() eq "stats" ) {
        print DBG_LOG "Found stats\n" if $debug > 0;
        if ( $self->is_stat_on() ) {
            $self->send_dods_stats();
        }
        exit(0);
    } elsif ( $self->ext() eq "help" ) {
        $self->print_help_message();
        exit(0);
    } elsif ( $self->ext() eq "/" ) {
        use FilterDirHTML;    # FilterDirHTML is a subclass of HTML::Filter

        # Build URL without CGI in it and use that to get the directory
        # listing from the web server.

        my $url =
          "http://" . $self->server_name() . $self->port() . $self->path_info();

        # Make sure URL ends in a slash. 10/12/2001 jhrg
        if ( $self->path_info() !~ m@^.*/$@ ) {
            print( DBG_LOG "In ext == `/', adding trailing slash.\n" )
              if $debug > 1;
            $url .= "/";
        }
        if ( $self->query() ne "" ) {
            $url .= "?" . $self->query();
        }

        print( DBG_LOG "Getting the directory listing using: $url\n" )
          if $debug > 1;

        my $directory_html = &get_url( $self, $url );

        print( DBG_LOG "Raw directory HTML:\n$directory_html\n\n" )
          if $debug > 1;

        # Parse the HTML directory page
        my $server_url = $self->full_uri();

        # Make sure server_url ends in a slash. 10/12/2001 jhrg
        if ( $server_url !~ m@^.*/$@ ) {
            print( DBG_LOG "In ext == `/', adding trailing slash(2).\n" )
              if $debug > 1;
            $server_url .= "/";
        }
        if ( $self->query() ne "" ) {
            ($server_url) = ( $server_url =~ m@(.*)\?.*@ );
        }
        my $excludes          = $self->exclude();
        my $filtered_dir_html =
          new FilterDirHTML( $server_url, $url,
                             dataset_regexes( "./dods.rc", @$excludes ) );

        # Print HTTP response headers. 06/25/04 jhrg
        print( STDOUT "HTTP/1.1 200 OK\n" );
        print( STDOUT "Content-Type: text/html\n\n" );

        $filtered_dir_html->parse($directory_html);
        $filtered_dir_html->eof;
        exit(0);    # Leave without returning @command!
    } elsif (    $self->ext() eq "das"
              || $self->ext() eq "dds"
              || $self->ext() eq "dods"
              || $self->ext() eq "ddx" )
    {
        @command = (
                     $self->handler(), "-v", $self->caller_revision(),
                     "-o", $self->ext(), "-u", $self->full_uri(),
                     $self->filename()
        );
        if ( $self->query() ne "" ) {
            @command = ( @command, "-e", $self->query() );
        }
        if ( $self->cache_dir() ne "" ) {
            @command = ( @command, "-r", $self->cache_dir() );
        }
        if ( $self->if_modified_since() != -1 ) {
            @command = ( @command, "-l", $self->if_modified_since() );
        }
        if ( $self->encoding() =~ /deflate/ ) {
            @command = ( @command, "-c" );
        }
        if ( $self->timeout() != 0 ) {
            @command = ( @command, "-t", $self->timeout() );
        }
    } elsif ( $self->ext() eq "ascii" || $self->ext() eq "asc" ) {
        my $dods_url = "http://"
          . $self->server_name()
          . $self->port()
          . $self->request_uri() . "?"
          . $self->query();
        @command = ( "asciival", "-m", "--", $dods_url );
    } elsif ( $self->ext() eq "netcdf" ) {
        my $dods_url = "http://"
          . $self->server_name()
          . $self->port()
          . $self->request_uri() . "?"
          . $self->query();
        @command = ( "dods2ncdf", "-m", "-p", "--", $dods_url );
    } elsif ( $self->ext() eq "html" ) {
        @command = (
                     "www_int", "-m", "-n", "--",
                     $self->full_uri() . "?" . $self->query()
        );
    } else {
        $self->print_dods_error( $unknown_ext, 1 );
        exit(1);
    }

    print( DBG_LOG "DODS server command: @command\n" ) if $debug;
    return @command;
}

my $DODS_Para1 = "The URL extension did not match any that are known by this
server. Below is a list of the extensions that are be recognized by
all DODS servers. If you think that the server is broken (that the URL you
submitted should have worked), then please contact the\n";

# Boilerplate text for the error messages.
my $DODS_Local_Admin = "administrator of this site at: ";
my $DODS_Support     = "DODS user support coordinator at: ";

my $DODS_Para2 = "To access most of the features of this DODS server, append
one of the following suffixes to a URL. Using these suffixes, you can ask 
this server for:<dl> 
<dt> das  <dd> attribute object
<dt> dds  <dd> data type object
<dt> dods <dd> data object
<dt> info <dd> info object (attributes, types and other information)
<dt> html <dd> html form for this dataset
<dt> ver  <dd> return the version number of the server
<dt> help <dd> help information (this text)</dl>
</dl>
For example, to request the DAS object from the FNOC1 dataset at URI/GSO (a
test dataset) you would append `.das' to the URL: http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc1.nc.das.

<p><b>Note</b>: Many DODS clients supply these extensions for you so you
should not append them when using interfaces supplied by us or software
re-linked with a DODS client-library. Generally, you only need to
add these if you are typing a URL directly into a WWW browser.

<p><b>Note</b>: If you would like version information for this server but
don't know a specific data file or data set name, use `/version' for the
filename. For example: http://dods.gso.uri.edu/cgi-bin/nph-dods/version will
return the version number for the netCDF server used in the first example. 

<p><b>Suggestion</b>: If you're typing this URL into a WWW browser and
would like information about the dataset, use the `.info' extension.

<p>If you'd like to see a data values, use the `.html' extension and submit a
query using the customized form.\n";

# Send the DODS version information.
sub send_dods_version {
    my $self         = shift;

    print "HTTP/1.0 200 OK\n";
    print "XDODS-Server: opendap/$self->caller_revision()\n";
    print "Content-Type: text/plain\n\n";

    print "OPeNDAP server core software: $self->caller_revision()\n";
}

# Send the DODS stats information. Only call this if is_stat_on() is true.
sub send_dods_stats {
    my $self         = shift;
    my $blessed = "unidata.ucar.edu|.*gso.uri.edu|.*dods.org|.*opendap.org";
    my $machine_names = $self->machine_names();

    print DBG_LOG "In send_dods_stats\n" if $debug > 0;
    if ( $self->server_name() =~ m@($blessed|$machine_names)@ ) {
        print "HTTP/1.0 200 OK\n";
        print "XDODS-Server: opendap/$self->caller_revision()\n";
        print "Content-Type: text/plain\n\n";

        print "Server: ", $self->server_name(), " (version: $self->caller_revision())\n";
        print DBG_LOG "Access log: ", $self->access_log(), "\n" if $debug > 0;
        &print_log_info( $self->access_log(), $self->error_log() );
    }
}

# Changed by adding the two new arguments. The first (after the `self'
# instance) is a variable that should name a string to print to report the
# error. The second is a flag that indicates (0 == false, no) whether to
# print the longish help message about URLs and their supported extensions.
# 6/8/2001 jhrg
sub print_error_message {
    my $self        = shift;
    my $msg         = shift;
    my $print_help  = shift;
    my $local_admin = 0;

    if ( $self->maintainer() ne "support\@unidata.ucar.edu" ) {
        $local_admin = 1;
    }

    # Note that 400 is the error code for `Bad Request'.

    print "HTTP/1.0 400 DODS server error.\n";
    print "XDODS-Server: DAP2/$self->{caller_revision}\n";
    my $time = gmtime;
    print "Date: $time GMT\n";
    print "Last-Modified: $time GMT\n";
    print "Content-type: text/html\n";
    print "Cache-Control: no-cache\n\n";
    print "\n";
    print "<h3>DODS Server or URL Error</h3>\n";

    print $msg;
    if ( $local_admin == 1 ) {
        print $DODS_Local_Admin, $self->maintainer();
    } else {
        print $DODS_Support, $self->maintainer();
    }
    print "<p>\n";

    print $DODS_Para2 if $print_help;

    my $date = localtime;
    print DBG_LOG "[$date] DODS Server error: ", $msg, "\n";
    if ( $local_admin == 1 ) {
        print DBG_LOG $DODS_Local_Admin, $self->maintainer(), "\n";
    } else {
        print DBG_LOG $DODS_Support, $self->maintainer(), "\n";
    }
}

sub print_dods_error {
    my $self        = shift;
    my $msg         = shift;
    my $local_admin = 0;

    if ( $self->maintainer() ne "support\@unidata.ucar.edu" ) {
        $local_admin = 1;
    }

    my $whole_msg;
    my $contact = $self->maintainer();
    if ( $local_admin == 1 ) {
        $whole_msg = "${msg}${DODS_Local_Admin}${contact}";
    } else {
        $whole_msg = "${msg}${DODS_Support}${contact}";
    }

    print DBG_LOG "Whole message: $whole_msg\n";

    print "HTTP/1.0 200 OK\n";
    print "XDODS-Server: DAP2/$self->{caller_revision}\n";
    my $time = gmtime;
    print "Date: $time GMT\n";
    print "Last-Modified: $time GMT\n";
    print "Content-type: text/plain\n";
    print "Content-Description: dods_error\n";
    print "Cache-Control: no-cache\n\n";

    print "Error {\n";
    print "    code = 1001;\n";
    print "    message = \"$whole_msg\";\n";
    print "};";

    my $date = localtime;
    print( DBG_LOG "[$date] DODS Server error: ", $whole_msg, "\n" );
}

# Assumption: If this message is being shown, it is probably being shown in a
# web browser. Use HTML to mark up the text. Other errors should be `marked
# up' using a DODS Error object.
sub print_help_message {
    my $self = shift;

    print "HTTP/1.0 200 OK\n";
    print "XDODS-Server: DAP2/$self->{caller_revision}\n";
    print "Content-Type: text/html\n";
    print "\n";

    print "<h3>DODS Server Help</h3>\n";

    print $DODS_Para2;
}

if ($test) {

    # Set up the environment variables used in initialize
    $ENV{SERVER_PORT}  = 80;
    $ENV{SERVER_NAME}  = "dcz.dods.org";
    $ENV{SERVER_ADMIN} = "jimbo";
    $ENV{QUERY_STRING} = "x,y,z&x<x&z>10.0";
    $ENV{PATH_INFO}    = "/data/x.nc.dods";
    $ENV{SCRIPT_NAME}  = "/test-3.2/nph-dods";

   # Replaced the use of this environment variable since it is not part of
   # the CGI 1.1 spec and not provided by Netscape's FastTrack server.
   # 4/30/2001 jhrg.
   # $ENV{REQUEST_URI} = "http://dcz.dods.org/test-3.2/nph-dods/data/x.nc.dods";
    $ENV{HTTP_ACCEPT_ENCODING} = "deflate";
    $self->data_path("/home/httpd/html/htdocs/data/x.nc.dods");

    print "Simple file access\n";
    my $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );
    $dd->ext()    eq "dods" || die;
    $dd->handler() eq "nc_handler"   || die;

    print "Files with extra dots on their names\n";

    # Test files which have more than one dot in their names.
    $ENV{PATH_INFO}       = "/data/tmp.x.nc.dods";
    $self->data_path("/home/httpd/html/htdocs/data/tmp.x.nc.dods");
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );

    $dd->ext()    eq "dods" || die;
    $dd->handler() eq "nc_handler"   || die;

    print "Directory names ending in a slash\n";

    # Directory ending in a slash.
    # NOTE: The directory must really exist!
    $ENV{PATH_INFO}       = "/data/";
    $self->data_path("/var/www/html/data/");
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );
    $dd->ext()    eq "/" || die;
    $dd->handler() eq ""  || die;    # a weird anomaly of handler.pm

    print "Directory names ending in a slash with a M=A query\n";

    # Directory ending in a slash with a query string
    $ENV{QUERY_STRING}    = "M=A";
    $ENV{PATH_INFO}       = "/data/";
#    $ENV{PATH_TRANSLATED} = "/var/www/html/data/";
    $self->data_path("/var/www/html/data/");
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );
    $dd->ext() eq "/" || die;

    print "Directory names not ending in a slash\n";

    # Directory, not ending in a slash
    $ENV{PATH_INFO}       = "/data";
    $ENV{PATH_TRANSLATED} = "/var/www/html/data";
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );
    $dd->ext() eq "/" || die;

    print "Directory names not ending in a slash with a M=A query\n";

    # Directory, not ending in a slash with a M=A query
    $ENV{QUERY_STRING}    = "M=A";
    $ENV{PATH_INFO}       = "/data";
    $self->data_path("/var/www/html/data");
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );
    $dd->ext() eq "/" || die;

    # Test the RFC822_to_time function.
    use POSIX;
    my $t = time;
    my ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
      gmtime($t);
    my $t_str = POSIX::strftime(
                                 "%a, %d %b %Y %H:%M:%S %z",
                                 $sec,  $min,  $hour, $mday, $mon,
                                 $year, $wday, $yday, $isdst
    );

    print "Time string in is: $t_str\n";
    my $t2 = rfc822_to_time($t_str);
    print "Time: $t, Time from converter: $t2\n";
    $t == $t2 || die;

    my $tt_str = POSIX::strftime(
                                  "%a %b %d %H:%M:%S %Y %z",
                                  $sec,  $min,  $hour, $mday, $mon,
                                  $year, $wday, $yday, $isdst
    );
    print "Time string in is: $tt_str\n";
    my $tt2 = rfc822_to_time($tt_str);
    print "Time: $t, Time from converter: $tt2\n";
    $t == $tt2 || die;

    print "Test is_directory()\n";

    # is_directory and is_file (just to be sure).
    is_directory(".")            || die;
    is_directory("..")           || die;
    is_directory("/")            || die;
    is_directory("/etc")         || die;
    is_directory("/etc/")        || die;
    !is_directory("/etc/passwd") || die;

    print "Test is_file()\n";
    !is_file(".")          || die;
    !is_file("..")         || die;
    !is_file("/")          || die;
    !is_file("/etc")       || die;
    !is_file("/etc/")      || die;
    is_file("/etc/passwd") || die;

    $ENV{PATH_INFO} =
      "/http://dcz.dods.org/dods-3.2/nph-dods/data/nc/fnoc1.nc.das";
    $self->data_path("/var/www/html$ENV{PATH_INFO}");
    $dd = new DODS_Dispatch( "dods/3.2.0", "jimg\@dcz.dods.org", "dods.rc" );

    # print "DODSter filename: $dd->filename() \n";
    # $dd->ext() eq "/" || die;
    # $dd->script() eq "" || die; # a weird anomaly of handler.pm

    print "All tests successful\n";
}

1;

# $Log: DODS_Dispatch.pm,v $
# Revision 1.47  2005/05/27 22:37:02  jimg
# Used the value of the configuration param data_root along with PATH_INFO
# to figure out where the data are. The data_root path is essentially a substitute
# for DocumentRoot. If data_root is not set in the config file, DocumentRoot is
# used in its place (by falling back to the old behavior of using PATH_TRANSLATED
# to find the data file).
#
# Revision 1.46  2005/05/25 23:53:43  jimg
# Changes to mesh with the new netcdf handler project/module. make install in
# both will now yield a running server when nph-dods and dods.rc are copied to
# a cgi-bin directory.
#
# Revision 1.45  2005/05/18 21:33:16  jimg
# Update for the new build/install.
#
# Revision 1.44  2005/04/18 17:04:34  pwest
# setting error code on an error to 1001, was being set to 0
#
# Revision 1.43  2004/10/22 17:54:38  jimg
# fule_uri() --> full_uri() when getting the version info. See bug 821.
#
# Revision 1.42  2004/07/07 21:17:54  jimg
# Merged with release-3-4-8FCS
#
# Revision 1.37.2.17  2004/06/25 19:50:12  jimg
# Added HTTP response status line and Content-Type header for the directory
# response.
#
# Revision 1.41  2004/01/22 17:29:13  jimg
# Merged with release-3-4.
#
# Revision 1.37.2.16  2004/01/12 20:06:49  jimg
# Turned off debugging for release of 3.4.
#
# Revision 1.40  2003/12/20 07:31:33  jimg
# Resolved conflicts with the merge of release-3-4.
#
# Revision 1.39  2003/12/08 18:04:06  edavis
# Merge release-3-4 into trunk
#
# Revision 1.37.2.15  2003/11/25 18:57:41  jimg
# Oops... Forgot to escape the '@' in a string.
#
# Revision 1.37.2.14  2003/11/21 20:12:05  jimg
# Fixed the error message produced when there's no valid URL suffix. See bug
# 680.
#
# Revision 1.37.2.13  2003/11/21 19:43:47  jimg
# Fixed the fix for the deflate bug (673). If the value of ACCEPT_ENCODING is
# not deflate, that's OK.
#
# Revision 1.37.2.12  2003/11/21 18:38:00  jimg
# Fixed the test for ACCEPT_ENCODING so that it'll work with string sent by
# Safari. I also made the error messages returned for malformed request headers
# more verbose and changed them from Web/HTML messages to OPeNDAP Error object
# responses (Which can be displayed as text and interpreted by the dap++
# library). See bug 673.
#
# Revision 1.37.2.11  2003/11/19 22:47:47  jimg
# I cleaned up the new() subroutine, removing some extraneous comments.
#
# Revision 1.37.2.10  2003/07/24 00:15:35  jimg
# Now uses the new/enhanced dods.rc configuration file (read via
# read_config.pm).
#
# Revision 1.37.2.9  2003/06/24 16:32:08  jimg
# Fixed bug 628. ACCPET_ENCODING needed to allow '-' because Konqueror uses
# x-gzip as one value.
#
# Revision 1.37.2.8  2003/06/23 23:29:36  jimg
# Fixed bug 625. When directory indexes were made using a URL that lacked a
# trailing slash, the filtered HTML was missing the last component of the
# pathname for filename links. Really odd. The initialize() function needs to
# be broken out into separate functions and probably so does comment(). There
# are too many places where initialize() depends on the value of a variable set
# twenty of more lines previously while computing some unrelated value.
#
# Revision 1.37.2.7  2003/06/18 20:11:43  jimg
# Completed(!) the fix for the curl-not-found-bug. Previously I patched
# installServers and fixed up the target that builds the server-tools tarball,
# but I forgot to actually fix the code... This script now assumes that curl
# has been installed in the server's directory.
#
# Revision 1.37.2.6  2003/06/18 06:00:01  jimg
# Added a note about characters that are shell meta characters. This might help
# writing patterns that sanitize environment variables, et cetera. Also added
# opendap.org to the list of domains that can access stats info. NB: bug 510
# was fixed in this file earlier; I completed the fix in dods_logging.pm today.
#
# Revision 1.37.2.5  2003/06/14 00:00:40  jimg
# Fix for bug 613; removed the SERVER_ADMIN code because that information is
# actually not used (maybe it should be, but it's not) and the way we sanitize
# that variable causes problems.
#
# Revision 1.37.2.4  2003/06/05 00:17:44  jimg
# Fixed (I Hope, it's hard to test and may depend on a particular httpd
# implementation and/or version) bug 610. The server port appeared twice in the
# URL asciival used to request data.
#
# Revision 1.37.2.3  2003/05/30 18:58:47  edavis
# Added $self as first parameter to several method calls that use $self
# but were not called with $self->xxx() form. Also corrected some method
# calls in test section, i.e., changed extension() to ext().
#
# Revision 1.38  2003/05/27 21:39:02  jimg
# Added test for 'ddx' extension.
#
# Revision 1.37.2.2  2003/05/08 01:36:06  jimg
# Turned off debugging...
#
# Revision 1.37.2.1  2003/05/07 23:34:09  jimg
# Fixes for curl and dids_dir/html/ascii responses.
#
# Revision 1.37  2003/05/02 16:28:45  jimg
# Switched to a DODS-specific log for the diagnostic messages. Also, boosted
# the filtering of environment variables so that taint mode works with perl
# 5.8.
#
# Revision 1.36  2003/04/28 23:56:08  jimg
# Fixes for Perl 5.8.0 taint mode. 5.8 seems to be stricter about tainted
# variables (which is good, I guess...).
#
# Revision 1.35  2003/04/23 23:26:27  jimg
# Merged with 3.3.1.
#
# Revision 1.34.2.2  2003/04/09 21:36:16  jimg
# Turned off debugging.
#
# Revision 1.34.2.1  2003/03/07 02:45:30  jimg
# Fixed a bug where URLs to a directory which do not end with a slash
# failed.
#
# Revision 1.34  2003/01/28 21:25:14  jimg
# Moved a fix from release-3-2 *by hand* here. The variable path_info was not
# being sanitized correctly. It's such a simple fix...
#
# Revision 1.33  2003/01/23 00:44:34  jimg
# Updated the copyrights on various source files. OPeNDAP is adopting the
# GNU Lesser GPL.
#
# Revision 1.32  2003/01/22 00:41:47  jimg
# Changed dods.ini to dods.rc.
#
# Revision 1.31  2003/01/22 00:12:05  jimg
# Added/Updated from release-3-2 branch.
#
# Revision 1.30  2002/12/31 22:28:45  jimg
# Merged with release 3.2.10.
#
# Revision 1.25.2.41  2002/12/30 01:46:05  jimg
#  Removed debugging.
#
# Revision 1.25.2.40  2002/11/05 00:40:12  jimg
# DODSter and JGOFS fixes.
#
# Revision 1.25.2.39  2002/10/24 01:03:11  jimg
# I improved the call to curl so that it uses the copy in ../bin if it's
# there, otherwise it uses the copy on PATH.
#
# Revision 1.25.2.38  2002/10/24 00:43:43  jimg
# Added changes that work with the dodster code. I replaced the LWP::get call
# with code that uses curl. It still needs some work...
#
# Revision 1.25.2.37  2002/08/20 16:09:12  edavis
# Reword .html and error message.
#
# Revision 1.25.2.36  2002/07/03 19:25:10  jimg
# I changed the set of DODS machines that can access stats information to
# any machine at gso.uri.edu or dods.org. The machine unidata.ucar.edu can
# still get the information.
#
# Revision 1.25.2.35  2002/06/06 01:04:25  jimg
# Cleared the debug flag.
#
# Revision 1.25.2.34  2002/06/06 00:53:45  jimg
# The info service now passes caller revision and cache directory information
# to usage so that it can be passed onto the filter programs. This enables the
# HDF server to use the cache dir set in the nph-dods CGI rather than having to
# fall back on the default. Using the default is bad because it's the data
# directory, a place where CGIs often don't have write privileges.
#
# Revision 1.25.2.33  2002/05/21 21:21:36  jimg
# Added code so that server log information can be accessed remotely.
#
# Revision 1.25.2.32  2002/04/22 15:57:52  jimg
# Added a line for the dods2nc filter. This is triggered by the .netcdf
# extension. Also added -m -n switches to the www_int call.
#
# Revision 1.25.2.31  2002/04/03 21:04:38  jimg
# Removed debugging (how'd that get checked in...)
#
# Revision 1.25.2.30  2002/04/03 13:53:41  jimg
# I added some instrumentation to the RFC_822 time string parsing code that
# prints a message to DBG_LOG when an unrecognized time string is sent. This was
# to help debug a problem reported a while ago about Mozilla's time strings.
# The problem seems to have gone away, but I thought the instrumentation was
# useful in its own right.
#
# Revision 1.25.2.29  2002/01/30 00:56:02  jimg
# Added comment about bug 334
#
# Revision 1.25.2.28  2001/12/27 21:17:53  jimg
# Directories with dots in their names broke again. I fixed this once (by
# adding a patch from Rob Morris) but it broke again further down in the code
# that looks at the return value from handler_name(). So now it's fixed again.
#
# Revision 1.25.2.27  2001/12/27 20:18:34  jimg
# Added Jason Thaxter's <thaxter@gomoos.org> patch for getting the server name
# from either the HTTP_HOST or SERVER_NAME env variables.
#
# Revision 1.25.2.26  2001/12/12 01:36:11  jimg
# Fixed a problem with directory names that don't end in slashes. These were
# being reported as `URLs with Bad characters in the extension.'
# Changed RFC822_to_time so that it recognizes more time strings.
# Added tests to cover the above changes/fixes.
#
# Revision 1.25.2.25  2001/10/14 00:42:32  jimg
# Merged with release-3-2-8
#
# Revision 1.25.2.24  2001/10/13 22:17:39  jimg
# *** empty log message ***
#
# Revision 1.25.2.23  2001/10/12 23:32:48  jimg
# Fixed a bug (#306) where clicking on a dataset link in the directory page
# fails because the URL is missing the slash that separates the file from the
# last directory.
#
# Revision 1.25.2.22  2001/10/10 23:10:46  jimg
# *** empty log message ***
#
# Revision 1.25.2.21  2001/10/02 00:45:50  jimg
# Removed Perl debugging.
#
# Revision 1.29  2001/09/28 20:30:11  jimg
# Merged with 3.2.7.
#
# Revision 1.25.2.20  2001/09/28 20:20:50  jimg
# Fixed an error in the command() method where $filename was tested with using
# is_directory() when $ext eq "/" should have been used.
#
# Revision 1.25.2.19  2001/09/26 22:27:47  dan
# Removed the regexp that stripped the PATH_INFO variable for the jg-dods
# filters.   Changes to jg-dods starting with version 3.2.2 require all the
# information that is available in PATH_INFO to allow relative directory
# searching to support multiple object dictionary files at a provider site.
#
# Revision 1.25.2.18  2001/09/19 20:37:59  jimg
# Fixed the error message displayed when no regex matches the dataset's
# extension.
#
# Revision 1.25.2.17  2001/07/19 22:22:04  jimg
# Turned off debugging for revision in CVS.
#
# Revision 1.25.2.16  2001/07/13 18:52:22  jimg
# Modified Rob's fix to use PATH_TRANSLATED and removed match looking for
# slashes.
#
# Revision 1.25.2.15  2001/07/12 22:07:09  jimg
# Fix from Rob Morris for directory names with `.' in them. The call to
# is_directory() was moved before the the line that uses a regex to separate
# the file's basename from its extension.
#
# Revision 1.25.2.14  2001/07/11 05:09:42  jimg
# Moved the (commented out) code that scanned pathnames for shell meta
# characters to DODS_Cache.pm. It is actually used there.
#
# Revision 1.28  2001/06/15 23:38:36  jimg
# Merged with release-3-2-4.
#
# Revision 1.25.2.13  2001/06/15 17:51:44  dan
# Removed redundant 'please contact' strings from error message.
#
# Revision 1.25.2.12  2001/06/15 00:55:20  jimg
# Fixed the directory listing generator. It's hard to test this other than
# running the code. $ext is set to "/" if the PATH_TRANSLATED information is a
# directory(). However, this code no longer uses regular expressions to figure
# out if the URL is a request for a directory listing. Instead the
# PATH_TRANSLATED info is tested using Perl's -d operator.
# I modified the use of the handler_name() function. It is only called when the
# handler name will actually be used. Thus if it returns "" that's always an
# error. Before it was always called, even when its return value was not used.
#
# Revision 1.25.2.11  2001/06/08 23:49:40  jimg
# Fixed the `version' and `help' extensions.
# Fixed `version' when the JG server is not installed. The dispatch script
# itself now processes `version.'
#
# Revision 1.25.2.10  2001/06/08 19:16:21  jimg
# Added a test for unrecognized dataset type.
# Error messages changed to DODS Error objects in most cases. Errors where the
# extension cannot be recognized are still reported as HTML documents because
# it is most likely that those will occur with a web browser.
#
# Revision 1.25.2.9  2001/05/18 16:06:45  jimg
# Added Rob's fix for the special case for the JGOFS server.
#
# Revision 1.25.2.8  2001/05/09 23:37:49  jimg
# Added a function that tests if a string names a directory on the host system.
# This function is now used to decide if a URL should be sent to the directory
# service. Thus we no longer need to end directory names with slashes.
#
# Revision 1.25.2.7  2001/05/09 23:10:00  jimg
# For the directory service, files routed through the HTML form generator
# are now chosen based on the regexes listed in dods.ini. It's possible to
# configure a given nph-dods to not use some of the expressions in the
# dods.ini file, so regexes like .* won't do odd things like route all files
# through the form interface. This is a partial fix, really, since the
# regexes still might include files that will cause the server to gag.
#
# Revision 1.25.2.6  2001/05/03 18:57:07  jimg
# Added code to extract the value of an If-Modified-Since header if it is
# present.
# Added support for DODSFilter's -l flag. This is used to pass the
# If-Modified-Since value to the server's filter programs.
#
# Revision 1.25.2.5  2001/04/30 19:46:19  jimg
# Replaced REQUEST_URI with other environment variables. REQUEST_URI is not
# part of the CGI 1.1 spec nor is it supported by the Netscape FastTrack server
# (see bug 111).
# Fixed use of localtime in debug and error messages so that it returns a
# string and not Unix time in seconds.
#
# Revision 1.25.2.4  2001/03/27 01:45:53  jimg
# Added code to special case the help and version fake dataset_ids. These now
# work but it's a kludge.
#
# Revision 1.25.2.3  2001/01/05 18:26:04  jimg
# Consolidated the regexps that sanitize $ext and $filename.
# Made error messages about bad a extension or filename exit the script.
#
# Revision 1.25.2.2  2001/01/04 17:43:28  jimg
# Added to the regexps that `sanitize' the filename and extension. These
# now correctly process directory requests.
#
# Revision 1.25.2.1  2000/12/11 20:40:08  jimg
# Added the is_tainted() subroutine; tests is a variable is tainted.
# Fixed $filename and $ext so that they are no longer tainted. See comments in
# the source.
#
# Revision 1.25  2000/10/19 23:50:37  jimg
# Moved the CVS Log to t eh end of the file.
# Added Shekhar's changes.
# Added a call to handler_name(); The DODS_Dispatch object now uses the
# handler_name() routine to choose which handler to use. This makes it possible
# to use a single dispatch script for all types of data served by dods.
#
# Revision 1.24  2000/08/02 22:20:23  jimg
# Merged with 3.1.8
#
# Revision 1.21.2.6  2000/06/01 21:24:43  jimg
# Added path_info method.
#
# Revision 1.21.2.5  2000/05/05 16:22:19  jimg
# Fixed a bug in port()
#
# Revision 1.21.2.4  2000/05/05 16:21:38  jimg
# Corrected some comments
#
# Revision 1.21.2.3  2000/05/02 22:46:05  jimg
# Fixed a bug (#18) where URLs with port numbers were mangled by the ASCII
# and html form options. The port number would be stripped of the URL when
# asciival or www_int fetched the DAS, DDS or DataDDS. To fix this I added
# a new field (server_port) and two new accessor functions. server_port()
# returns the port number; port() returns a null string if the port is 80 or
# ":<port num>" for any other number.
#
# Revision 1.23  2000/01/27 17:54:03  jimg
# Merged with release-3-1-4
#
# Revision 1.21.2.2  2000/01/11 19:09:34  jimg
# Added code to check for a trailing / and bypass a bogus error message when
# the directory name contained characters that are not allowed in the URL
# extension used to identify a DODS object. This means that the directory
# listing will work for directories whose names contain underscores, numbers,
# etc.
#
# Revision 1.22  1999/11/04 23:59:57  jimg
# Result of merge with 3-1-3
#
# Revision 1.21.2.1  1999/10/19 17:35:33  jimg
# Read the server admin environment variable and pass its value to www_int.
#
# Revision 1.21  1999/07/30 19:59:08  jimg
# Added directory code from non-cvs version
#
# Revision 1.19  1999/07/22 03:08:46  jimg
# Moved
#
# Revision 1.18  1999/06/12 00:00:26  jimg
# Added server_name and request_uri procedure/fields.
# Added code that translates the extension .html into a call to the www_int
# filter. This uses the new server_name and request_uri information to run the
# filter with a complete url, which might be wasteful but is quick and keeps
# the server programs modular.
# Fixed the call to asciival so that security features are no longer
# circumvented. I used the same call-with-url technique that I used with
# www_int.
# Some of the help text was updated to include mention of the new .html
# feature.
#
# Revision 1.17  1999/05/27 21:27:59  jimg
# Moved the code that escapes the query into the section for asciival. Since
# the security fixes only asciival needs special characters escaped (since it
# is still run using a subshell).
#
# Revision 1.16  1999/05/24 23:34:35  dan
# Added support for JGOFS dispatch script, which requires
# filename = PATH_INFO, not filename = PATH_TRANSLATED
#
# Revision 1.15  1999/05/21 20:05:11  jimg
# Retracted some of the security stuff when using the ASCII mode of the
# servers. In order to run a pipe from Perl you must use an intermediate shell
# or (maybe) explicitly open the two processes using open("|", ...). The later
# might work but does not fit well into the design of DODS_Dispatch.pm. In the
# long run, we'll have to change DODS_Dispatch, but for now I'm running that
# part of the server through a shell.
#
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
# Added to the accessor functions so that they can be used to set the field
# values in addition to reading values from the `object'.
#
# Revision 1.1  1997/06/02 21:04:35  jimg
# First version
