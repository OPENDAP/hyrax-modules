
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

# Filter the directory page so that links are sent to my CGIs and not the
# browser directly. Directories are routed through the query_dir.pl CGI,
# files ending in $ext are sent to the query_form.pl CGI. Plain files are
# left as is. 8/11/98 jhrg

# Fixed link to parent directory. 8/12/98 jhrg

# The Filter.pm code is now (2006) deprecated. But I hope we will replace this
# 'system' of filtering HTML with THREDDS catalogs very soon. 2/23/06 jhrg.

# Set this to 1, 2 to get varius levels of instrumentation sent to stderr.
my $debug = 0;
my $test  = 0;

# Add here patterns that describe files that should *not* be treated as data
# files (and thus should not be routed through the DODS HTML form generator).
# 6/15/99 jhrg
#
# These are not needed anymore unless we let a `.*' regex into the
# dataset_regexes passed in to the ctor. The code which builds up this list
# can exclude certain regexes (like the JGOFS ones) that cause problems
# becuase they flag as datasets just about anything. 5/9/2001 jhrg
my @pass_through_patterns = ('.*\?[A-Z]=[A-Z]');

# old value. From when pass_through_patterns was used differently. 5/9/2001
# jhrg
# ('README', '.*\.html', '.*\.d[da]s', '.*\.ovr.*', '.*\?[A-Z]=[A-Z]');

package FilterDirHTML;
require HTML::Filter;
@ISA = qw(HTML::Filter);

# The ctor takes three arguments, the server's URL, the directory's URL (both
# scalars) and a list of regexs that define that types of files this server
# recognizes as datasets (or comprising datasets). The later is probably
# derived from the dap-server.rc file. Note that the directory URL is used to
# build references to files found in the data directories that should not be
# routed through the DODS server. For example, it's an error to run a regulat
# html file through a DODS server. 5/9/2001 jhrg

sub initialize {
    my $self = shift;
    my ( $server, $directory, @dataset_regexes ) = @_;

    print STDERR "dataset_regexes: @dataset_regexes\n" if $debug > 1;

    # $dataset_regexes holds a list of regexes used to choose which files in
    # a directory are *data* files and should be routed to the HTML form
    # interface.
    $self->{dataset_regexes} = \@dataset_regexes;

    # URL fragment that names the server.
    $self->{server} = $server;

    # URL that names the directory page being processed.
    $self->{directory} = $directory;

    # True (1) when the first anchor has been processed. The first anchor is
    # a link to the CWD's parent. We need to do special processing to get
    # this one correct.
    $self->{first_anchor_processed} = 0;

    # Assume that this is an index unless we find out otherwise (see method
    # `text'.
    $self->{is_index} = 1;
}

#  This method is called when a complete start tag has been
#  recognized. The first argument is the tag name (in lower
#  case) and the second argument is a reference to a hash that
#  contain all attributes found within the start tag. The
#  attribute keys are converted to lower case. Entities found
#  in the attribute values are already expanded. The third
#  argument is a reference to an array with the lower case
#  attribute keys in the original order. The fourth argument is
#  the original HTML text.
#
# My version of this method and its companion (see end) check for the title,
# body, h1 and a tags. When they are found the flag <tag>_seen is set (here)
# and cleared (in end). This sets the stage for the real filtering operations
# (see the text and output methods). 6/14/99 jhrg
#
sub start {
    my $self = shift;

    #print STDERR "Inside start\n";

    # If this is not an index then do nothing.
    if ( !$self->{is_index} ) {
        $self->SUPER::start(@_);
        return;
    }

    my ( $tag, $attr, $attrseq, $origtext ) = @_;

    print STDERR "tag: " . $tag . "\n" if $debug > 0;

    if ( $tag eq "title" ) {
        $self->{title_seen} = 1;
    }
    elsif ( $tag eq "body" ) {
        $self->{body_seen} = 1;
    }
    elsif ( $tag eq "h1" ) {
        $self->{h1_seen} = 1;
    }
    elsif ( $tag eq "a" ) {
        print STDERR "Found and anchor! $attr->{href}\n" if $debug > 0;
        $self->{anchor_seen} = 1;
        $self->{anchor_href} = $attr->{href};
    }

    $self->SUPER::start(@_);
}

#  This method is called when an end tag has been recognized.
#  The first argument is the lower case tag name, the second
#  the original HTML text of the tag.
sub end {
    my $self = shift;

    # If this is not an index then do nothing.
    if ( !$self->{is_index} ) {
        $self->SUPER::end(@_);
        return;
    }

    my ( $tag, $origtext ) = @_;

    print STDERR "tag: $tag \n" if $debug > 0;

    $self->SUPER::end(@_);
    if ( $tag eq "title" ) {
        $self->{title_seen} = 0;
    }
    elsif ( $tag eq "body" ) {
        $self->{body_seen} = 0;
    }
    elsif ( $tag eq "h1" ) {
        $self->{h1_seen} = 0;
    }
    elsif ( $tag eq "a" ) {
        $self->{anchor_seen} = 0;
    }
}

#  This method is called when plain text in the document is
#  recognized. The text is passed on unmodified and might
#  contain multiple lines. Note that for efficiency reasons
#  entities in the text are not expanded. You should call
#  HTML::Entities::decode($text) before you process the text
#  any further.
#
# This method rewrites the `Index of...' title and text so that it says `DODS
# Index of...'. Also not that this is place where we find out that the HTML
#  we're parsing might not actually be an index. 6/14/99 jhrg
#
sub text {
    my $self = shift;

    # If this is not an index then do nothing.
    if ( !$self->{is_index} ) {
        $self->SUPER::text(@_);
        return;
    }

    my ($text) = @_;

    # Changes the title that says `Index of...' to `DODS Index of...'
    if ( $self->{title_seen} ) {
        if ( $text !~ /Index of .*/ ) {

            # If this is not an index then clear the is_index property
            $self->{is_index} = 0;
        }
        else {
            $text = "OPeNDAP Server " . $text;
        }
    }

    # Changes the text/heading that says `Index of...' to `DODS Index of...'
    elsif ( $self->{body_seen} && $self->{h1_seen} ) {
        $text = "OPeNDAP Server " . $text;
    }

    $self->SUPER::text($text);
}

sub output {
    my $self            = shift;
    my $dataset_regexes = $self->{dataset_regexes};

    print STDERR "self->{anchor_ref}: $self->{anchor_ref}\n" if $debug > 2;
    print STDERR "self->{server}: $self->{server}\n"         if $debug > 2;

    # If this is not an index then do nothing.
    if ( !$self->{is_index} ) {
        $self->SUPER::output(@_);
        return;
    }

    if ( $self->{anchor_seen} ) {
        $self->{anchor_seen} = 0;
        my $new_anchor;

        # Is the anchor's href a directory (does it end in /)?
        if ( $self->{anchor_href} =~ /.*\/$/ ) {

            # Assume that the first anchor is a back link to the parent
            # directory. Thus, instead of catenating the base url with the
            # anchor href, make a href by `backing up' one level in the base
            # url. Note that the special case where the href == / must be
            # taken into account or the script will `go above' the htdocs
            # root.
            if ( !$self->{first_anchor_processed} ) {
                my ( $parent, $child ) = ( $self->{server} =~ m@(.*/)(.*/)$@ );
                $new_anchor = "<A HREF=\"" . $parent . "\">";
                $self->{first_anchor_processed} = 1;
            }
            else {
                $new_anchor =
                  "<A HREF=\"" . $self->{server} . $self->{anchor_href} . "\">";
            }
        }

        # These are anchors that are not datasets but still should be passed
        # through the DODS server when dereferenced. 5/9/2001 jhrg
        elsif ( grep { $self->{anchor_href} =~ $_ } @pass_through_patterns ) {
            $new_anchor = "<A HREF=\"" . $self->{anchor_href} . "\">";
            print STDERR
              "Anchor matches a pass through pattern: $self->{anchor_href}\n"
              if $debug > 0;
        }

        # Is the href a data file? If so append .html to the file name after
        # building a full URL.
        elsif ( grep { $self->{anchor_href} =~ $_ } @$dataset_regexes ) {
            $new_anchor =
                "<A HREF=\""
              . $self->{server}
              . $self->{anchor_href}
              . ".html\">";
            print STDERR
"Anchor matches extension pattern ($self->{dataset_regexes}): $self->{anchor_href}\n"
              if $debug > 0;
        }

        # Is it a regular file?
        else {
            $new_anchor =
              "<A HREF=\"" . $self->{directory} . $self->{anchor_href} . "\">";
            print STDERR
"Anchor is a regular file: $self->{directory}$self->{anchor_href}\n"
              if $debug > 0;
        }

        $self->SUPER::output($new_anchor);
    }
    else {
        $self->SUPER::output(@_);
    }
}

if ($test) {

    my $directory_html = "<html>
 <head>
  <title>Index of /data</title>
 </head>
 <body>
<h1>Index of /data</h1>
<pre><img src=\"/icons/blank.gif\" alt=\"Icon \" /> <a href=\"?C=N;O=D\">Name</a>                <a href=\"?C=M;O=A\">Last modified</a>      <a href=\"?C=S;O=A\">Size</a>  <a href=\"?C=D;O=A\">Description</a><hr /><img src=\"/icons/back.gif\" alt=\"[DIR]\" /> <a href=\"/\">Parent Directory</a>                             -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"dsp/\">dsp/</a>        10-Jan-2006 13:33    -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"ff/\">ff/</a>       10-Jan-2006 13:33    -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"hdf/\">hdf/</a>        10-Jan-2006 13:33    -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"jg/\">jg/</a>       10-Jan-2006 13:32    -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"mat/\">mat/</a>        10-Jan-2006 13:33    -
<img src=\"/icons/folder.gif\" alt=\"[DIR]\" /> <a href=\"nc/\">nc/</a>       10-Jan-2006 13:34    -
<hr /></pre>
<address>Apache/2.0.46 (Red Hat) Server at test.opendap.org Port 80</address>
</body></html>";

    print("Raw directory HTML:\n$directory_html\n\n")
      if $debug > 1;

    my $server_url        = "http://localhost/opendap/nph-dods/";
    my $url               = "/data/nc/";
    my $regexes           = "\.*\\.(HDF|hdf|EOS|eos)(\.Z|\.gz|\.bz2)?\$";
    my $filtered_dir_html = FilterDirHTML->new;

    $filtered_dir_html->initialize( $server_url, $url, $regexes );

    # Print HTTP response headers. 06/25/04 jhrg
    print( STDOUT "HTTP/1.1 200 OK\n" );
    print( STDOUT "Content-Type: text/html\n\n" );
    $filtered_dir_html->parse($directory_html);
    $filtered_dir_html->eof;
}


1;
