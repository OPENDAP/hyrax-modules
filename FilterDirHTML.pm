
# Filter the directory page so that links are sent to my CGIs and not the
# browser directly. Directories are routed through the query_dir.pl CGI,
# files ending in $ext are sent to the query_form.pl CGI. Plain files are
# left as is. 8/11/98 jhrg

# Fixed link to parent directory. 8/12/98 jhrg

# $Log: FilterDirHTML.pm,v $
# Revision 1.6  1999/06/22 17:08:10  jimg
# Added comments to describe what the overloads do.
# Reduced the number of parameters given to the ctor.
# Modified so that README, .html and directory sorting URLs are not routed to
# the html form filter.
# This code is now called from DODS_Dispatch.pm.
#
# Revision 1.5  1999/06/10 23:52:49  jimg
# Fixed the `parent directory' link.
#
# Revision 1.4  1998/11/19 03:32:35  jimg
# Changed from `Dataurl' to `url' to match the query_form.pl code.
#
# Revision 1.3  1998/09/29 22:58:29  jimg
# Fixed a bug where the protocol and machine part of a dataset URL were
# repeated (meaning that they appeared twice in the same URL).
#
# Revision 1.2  1998/08/12 18:20:31  jimg
# Added the `server' param to those that are passed to sub query_dir.pl
# calls. This fixes a bug where going into nested directories caused dataset
# URLs to be built without server information.
#
# Revision 1.1  1998/08/12 18:11:13  jimg
# Initial revision.
#

# Set this to 1, 2 to get varius levels of instrumentation sent to stderr.
my $debug = 0;

# Add here patterns that describe files that should *not* be treated as data
# files (and thus should not be routed through the DODS HTML form generator).
# 6/15/99 jhrg
my @pass_through_patterns = ('README', '.*\.html', '.*\?[A-Z]=[A-Z]');

package FilterDirHTML;
require HTML::Filter;
@ISA=qw(HTML::Filter);

sub new {
    my $this = shift;
    my ($ext, $server) = @_;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;

    # $ext holds a regex used to choose which files in a directory are *data*
    # files and should be routed to the query_form.pl CGI.
    $self->{ext} = $ext;

    # URL fragment that names the server for the given extension.
    $self->{server} = $server;

    # True (1) when the first anchor has been processed. The first anchor is
    # a link to the CWD's parent. We need to do special processing to get
    # this one correct.
    $self->{first_anchor_processed} = 0;

    # Assume that this is an index unless we find out otherwise (see method
    # `text'.
    $self->{is_index} = 1;

    return $self;
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

    # If this is not an index then do nothing.
    if (!$self->{is_index}) {
	$self->SUPER::start(@_);
	return;
    }
	
    my($tag, $attr, $attrseq, $origtext) = @_;

    print "tag: " . $tag . "\n" if $debug > 0;

    if ($tag eq "title") {
	$self->{title_seen} = 1;
    }
    elsif ($tag eq "body") {
	$self->{body_seen} = 1;
    }
    elsif ($tag eq "h1") {
	$self->{h1_seen} = 1;
    }
    elsif ($tag eq "a") {
	print "Found and anchor! $attr->{href}\n" if $debug > 0;
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
    if (!$self->{is_index}) {
	$self->SUPER::end(@_);
	return;
    }

    my ($tag, $origtext) = @_;

    print "tag: " . $tag . "\n" if $debug > 0;

    $self->SUPER::end(@_);
    if ($tag eq "title") {
	$self->{title_seen} = 0;
    }
    elsif ($tag eq "body") {
	$self->{body_seen} = 0;
    }
    elsif ($tag eq "h1") {
	$self->{h1_seen} = 0;
    }
    elsif ($tag eq "a") {
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
    if (!$self->{is_index}) {
	$self->SUPER::text(@_);
	return;
    }
	
    my ($text) = @_;

    # Changes the title that says `Index of...' to `DODS Index of...'
    if ($self->{title_seen}) {
	if ($text !~ /Index of .*/) {
	    # If this is not an index then clear the is_index property
	    $self->{is_index} = 0;
	}
	else {
	    $text = "DODS " . $text;
	}
    }
    # Changes the text/heading that says `Index of...' to `DODS Index of...'
    elsif ($self->{body_seen} && $self->{h1_seen}) {
	$text = "DODS " . $text;
    }

    $self->SUPER::text($text);
}

sub output {
    my $self = shift;

    # If this is not an index then do nothing.
    if (!$self->{is_index}) {
	$self->SUPER::output(@_);
	return;
    }

    if ($self->{anchor_seen}) {
	$self->{anchor_seen} = 0;
	my $new_anchor;

	# Is the anchor's href a directory (does it end in /)?
	if ($self->{anchor_href} =~ /.*\/$/) {
	    # Assume that the first anchor is a back link to the parent
	    # directory. Thus, instead of catenating the base url with the
	    # anchor href, make a href by `backing up' one level in the base
	    # url. Note that the special case where the href == / must be
	    # taken into account or the script will `go above' the htdocs
	    # root. 
	    if (!$self->{first_anchor_processed}) {
		my ($parent, $child) = ($self->{server} =~ m@(.*/)(.*/)$@);
		$new_anchor = "<A HREF=" . $parent . ">";
		$self->{first_anchor_processed} = 1;
	    }
	    else {
		$new_anchor = "<A HREF=" . $self->{server}
		              . $self->{anchor_href} . ">"; 
	    }
	}
	# Test special case files which will pass the extension test is the
	# extension is `.*'
	elsif (map { $self->{anchor_href} =~ $_ } @pass_through_patterns) {
 	    $new_anchor = "<A HREF=" . $self->{anchor_href} . ">"; 
	}
	# Is the href a data file? If so append .html to the file name after
	# building a full URL.
	elsif ($self->{anchor_href} =~ /.*$self->{ext}$/) {
	    $new_anchor = "<A HREF=" . $self->{server} . $self->{anchor_href}
	                  . ".html>"; 
	}
	# Is it a regular file?
	else {
 	    $new_anchor = "<A HREF=" . $self->{anchor_href} . ">"; 
	}

	$self->SUPER::output($new_anchor);
    }
    else {
	$self->SUPER::output(@_);
    }
}
