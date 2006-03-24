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

my $debug = 0;
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
# SCRIPT_NAME: Used to build the request_uri field's value. Used by asciival,
# et cetera.
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

    open( DBG_LOG, ">> ./dbg_log" ) if $debug;
    
    print( DBG_LOG "------------------------------------------------\n" ) if $debug;
    print( DBG_LOG "DODS Server debug log: ", scalar localtime, "\n" ) if $debug;

    # Read the configuration file.
    my ( $timeout, $cache_dir, $cache_size, $maintainer, $curl, $usage,
	 $www_int, $asciival, $data_root, @exclude ) 
	= get_params( $self->{config_file} );

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
    $self->usage($usage);
    $self->www_int($www_int);
    $self->asciival($asciival);

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
    print( DBG_LOG "QS: ", $self->{query}, "\n" ) if $debug > 1;

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
    } elsif ( $ext =~ /^.*\.(das|dds|ddx|dods|ascii|asc|version|ver|info|html)$/ ) {
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
    # make sure that the server (via dap-server.rc) is configured for the
    # particular type of URL. If we don't do that then an errant request for
    # .html, for example, will loop forever (since it's a subordinate request
    # that accesses the dataset and that's what fails). 9/19/2001 jhrg
    #
    # Slight modification: If the handler is null ("") and the extension is a
    # slash ("/"), that's OK. See Bug 334. 12/27/2001 jhrg

    # $self->{handler} = handler_name( $self->{path_info}, $self->{config_file} );

    # This code knows how to read 0 .. n command line switche/params from the 
    # dap-server.rc config file, but I have not modified this file to pass those
    # into the various handlers.
    ($self->{handler}, $self->{handler_switches})
	   = handler_name( $self->{path_info}, $self->{config_file} );
    
    if (    $self->{ext} ne "/"
         && $self->{ext} ne "stats"
         && $self->{ext} ne "version"
         && $self->{ext} ne "help"
         && $self->{handler} eq "" )
    {
        $self->print_dods_error( "${unknown_p1}${path_info}${unknown_p2}", 0 );
        exit(1);
    }

    print DBG_LOG "Server type: $self->{handler}, $self->{handler_switches}\n"
        if $debug > 1;

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
# names' (see the dap-server.rc file) that have regular expressions which
# should NOT be rerouted through the DODS server's HTML form generator. Often
# this is the case because their regexes are something like `.*'. 5/9/2001
# jhrg The 'exclude' stuff is now handled by a setting in the dap-server.rc
# file. 11/19/03 jhrg
#
# At some point a fourth param was added so that it would be possible to pass
# into this object the name of the configuration file. 10/21/02 jhrg Now the
# dap-server.rc handles the parameters and so the other two arguments went
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
    # my $value = shift;    # Optional, use to set.

    if ( $#_ == -1 ) {
        return $self->{maintainer};
    } else {
        return $self->{maintainer} = shift;
    }
}

sub exclude {
    my $self   = shift;
    # my @values = @_;

    if ( $#_ == -1 ) {
        return $self->{exclude};
    } else {
        return $self->{exclude} = shift;
    }
}

# Return the query string given with the URL.
sub query {
    my $self  = shift;
    # my $query = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{query};
    } else {
        return $self->{query} = shift;
    }
}

# If the second argument is given, use it to set the filename member.
sub filename {
    my $self     = shift;
    # my $filename = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{filename};
    } else {
        return $self->{filename} = shift;
    }
}

sub ext {
    my $self      = shift;
    # my $extension = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{ext};
    } else {
        return $self->{ext} = shift;
    }
}

sub cgi_dir {
    my $self    = shift;
    # my $cgi_dir = shift;      # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{cgi_dir};
    } else {
        return $self->{cgi_dir} = shift;
    }
}

sub cache_dir {
    my $self      = shift;
    # my $cache_dir = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{cache_dir};
    } else {
        return $self->{cache_dir} = shift;
    }
}

sub cache_size {
    my $self       = shift;
    #my $cache_size = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{cache_size};
    } else {
        return $self->{cache_size} = shift;
    }
}

sub timeout {
    my $self    = shift;
    # my $timeout = shift;       # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{timeout};
    } else {
        return $self->{timeout} = shift;
    }
}

sub curl {
    my $self = shift;
    # my $curl = shift;          # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{curl};
    } else {
        return $self->{curl} = shift;
    }
}

sub usage {
    my $self = shift;
    # my $usage = shift;          # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{usage};
    } else {
        return $self->{usage} = shift;
    }
}

sub www_int {
    my $self = shift;
    # my $www_int = shift;          # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{www_int};
    } else {
        return $self->{www_int} = shift;
    }
}

sub asciival {
    my $self = shift;
    # my $asciival = shift;          # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{asciival};
    } else {
        return $self->{asciival} = shift;
    }
}

sub access_log {
    my $self       = shift;
    # my $access_log = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{access_log};
    } else {
        return $self->{access_log} = shift;
    }
}

sub error_log {
    my $self      = shift;
    # my $error_log = shift;     # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{error_log};
    } else {
        return $self->{error_log} = shift;
    }
}

sub machine_names {
    my $self          = shift;
    # my $machine_names = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{machine_names};
    } else {
        return $self->{machine_names} = shift;
    }
}

sub is_stat_on {
    my $self  = shift;
    # my $value = shift;

    if ( $#_ == -1 ) {
        return $self->{is_stat_on};
    } else {
        return $self->{is_stat_on} = shift;
    }
}

# This returns the full path to the handler. The set of known handlers
# is read from the dap-server.rc file during initialization. 
sub handler {
    my $self   = shift;
    # my $handler = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{handler};
    } else {
        return $self->{handler} = shift;
    }
}

# This returns the directory where the server binaries are stored. From the 
# configuration file (dap-server.rc).
sub bin_dir {
    my $self   = shift;
    # my $bin_dir = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{bin_dir};
    } else {
        return $self->{bin_dir} = shift;
    }
}

sub data_path {
    my $self   = shift;
    # my $data_path = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{data_path};
    } else {
        return $self->{data_path} = shift;
    }
}

sub handler_switches {
    my $self   = shift;
    # my $data_path = shift;    # The second arg is optional

    if ( $#_ == -1 ) {
        return $self->{handler_switches};
    } else {
        return $self->{handler_switches} = shift;
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

sub url_text {
    my $self = shift;

    my $url = "http://"
	. $self->server_name()
	. $self->port()
	. $self->request_uri();
	
	if ($self->query()) {
	    $url .= "?" . $self->query();
	}
	
    return $url
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
        $options    = "'-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir() . "'";
        }
        else {
            $options .= "'";
        }

        @command = ( $self->usage(), $options, $self->filename(), $self->handler() );
    } elsif ( $self->ext() eq "ver" || $self->ext() eq "version" ) {

        # if there's no filename assume `.../nph-dods/version/'. 6/8/2001 jhrg
        if ( $self->filename() eq "" ) {
            $self->send_dods_version();
            exit(0);
        } else {
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
        my $filtered_dir_html = FilterDirHTML->new;
        $filtered_dir_html->initialize( $server_url, $url, dataset_regexes( "./dap-server.rc", @$excludes ) );

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
                     $self->filename(), $self->handler_switches()
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
        $options    = "-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir();
        }

        @command = ( $self->asciival(), $options, "-m", 
		     "-u", $self->url_text(),
		     "-f", $self->handler(), 
		     "--", $self->filename() ); #. "?" . $self->query() );
    } elsif ( $self->ext() eq "html" ) {
        $options    = "-v " . $self->caller_revision() . " ";
        if ( $self->cache_dir() ne "" ) {
            $options .= "-r " . $self->cache_dir();
        }

        @command = ( $self->www_int(), $options, "-m", "-n", 
		     "-u", $self->url_text(),
		     "-f", $self->handler(), 
		     "--", $self->filename() ); #. "?" . $self->query() );
    } elsif ( $self->ext() eq "netcdf" ) {
        my $dods_url = "http://"
          . $self->server_name()
          . $self->port()
          . $self->request_uri() . "?"
          . $self->query();
        @command = ( "dods2ncdf", "-m", "-p", "--", $self->url_text() );
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
    # For soe reason the function call doesn't work ina string but variable
    # substitution does... jhrg 7/22/05
    my $version      = $self->caller_revision();

    print "HTTP/1.0 200 OK\n";
    print "XDODS-Server: opendap/$self->caller_revision()\n";
    print "Content-Type: text/plain\n\n";

    print "OPeNDAP server core software: $version\n";
}

# Send the DODS stats information. Only call this if is_stat_on() is true.
sub send_dods_stats {
    my $self         = shift;
    my $machine_names = $self->machine_names();
    my $version      = $self->caller_revision(); # See above comment.

    print DBG_LOG "In send_dods_stats\n" if $debug > 0;
    if ( $self->server_name() =~ m@($machine_names)@ ) {
        print "HTTP/1.0 200 OK\n";
        print "XDODS-Server: opendap/$self->caller_revision()\n";
        print "Content-Type: text/plain\n\n";

        print "Server: ", $self->server_name(), " (version: $version)\n";
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
    $ENV{PATH_INFO}    = "/data/nc/x.nc.dods";
    $ENV{SCRIPT_NAME}  = "/test-3.2/nph-dods";
    $ENV{PATH_TRANSLATED} = "/usr/local/dap_data/data/nc/x.nc.dods";

    my $dd = new DODS_Dispatch( "2.0", "./dap-server.rc" );
    print $dd->ext(), "\n";
    print $dd->handler(), "\n";
    # print $dd->handler_switches(), "\n";
    
    print $dd->command(), "\n";

   # Replaced the use of this environment variable since it is not part of
   # the CGI 1.1 spec and not provided by Netscape's FastTrack server.
   # 4/30/2001 jhrg.
   # $ENV{REQUEST_URI} = "http://dcz.dods.org/test-3.2/nph-dods/data/x.nc.dods";
    $ENV{HTTP_ACCEPT_ENCODING} = "deflate";
    # $self->data_path("/home/httpd/html/htdocs/data/x.nc.dods");

    print "Simple file access\n";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
    $dd->ext()    eq "dods" || die;
    $dd->handler() eq "/usr/local/bin/dap_nc_handler"   || die;

    print "Files with extra dots on their names\n";

    # Test files which have more than one dot in their names.
    $ENV{PATH_INFO}       = "/data/tmp.x.nc.dods";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
    $dd->ext()    eq "dods" || die;
    $dd->handler() eq "/usr/local/bin/dap_nc_handler"   || die;

    print "Directory names ending in a slash\n";

    # Directory ending in a slash.
    # NOTE: The directory must really exist!
    $ENV{PATH_INFO}       = "/data/nc/";
    # The following must be a real path on the system running the tests
    $ENV{PATH_TRANSLATED} = "/usr/local/dap_data/data/nc/";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
    $dd->ext()    eq "/" || die;
    $dd->handler() eq ""  || die;    # a weird anomaly of handler.pm

    print "Directory names ending in a slash with a M=A query\n";

    # Directory ending in a slash with a query string
    $ENV{QUERY_STRING}    = "M=A";
    $ENV{PATH_INFO}       = "/data/nc/";
    $ENV{PATH_TRANSLATED} = "/usr/local/dap_data/data/nc/";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
    $dd->ext() eq "/" || die;

    print "Directory names not ending in a slash\n";

    # Directory, not ending in a slash
    $ENV{PATH_INFO}       = "/data/nc";
    $ENV{PATH_TRANSLATED} = "/usr/local/dap_data/data/nc";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
    $dd->ext() eq "/" || die;

    print "Directory names not ending in a slash with a M=A query\n";

    # Directory, not ending in a slash with a M=A query
    $ENV{QUERY_STRING}    = "M=A";
    $ENV{PATH_INFO}       = "/data";
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );
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
    $dd = new DODS_Dispatch( "2.0", "dap-server.rc" );

    print "All tests successful\n";
}

1;
