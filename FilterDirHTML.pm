
# Filter the directory page so that links are sent to my CGIs and not the
# browser directly. Directories are routed through the query_dir.pl CGI,
# files ending in $ext are sent to the query_form.pl CGI. Plain files are
# left as is. 8/11/98 jhrg

# Fixed link to parent directory. 8/12/98 jhrg

# $Log: FilterDirHTML.pm,v $
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

package FilterDirHTML;
require HTML::Filter;
@ISA=qw(HTML::Filter);

sub new {
    my $this = shift;
    my ($ext, $server, $base_url, $dir_cgi, $query_cgi) = @_;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;

    # $ext holds a regex used to choose which files in a directory are *data*
    # files and should be routed to the query_form.pl CGI.
    $self->{ext} = $ext;

    # URL fragment that names the server for the given extension.
    $self->{server} = $server;

    # The base url of the current directory. Apache (et al.?) emits directory
    # pages with relative URLS. Because we are routing those URLS through a
    # processing script they need to be absolute. Note that this means that
    # the FilterDirHTML instance must be started with the correct path.
    $self->{base_url} = $base_url;

    # URL of the directory CGI. Route URLs that reference directories through
    # this. 
    $self->{dir_cgi} = $dir_cgi . "?ext=" . $ext . "&server=" . $server .
	"&url=";


    # URL of the query form CGI. Route URLs for datasets through this.
    $self->{query_cgi} = $query_cgi . "?Dataurl=";

    # True (1) when the first anchor has been processed. The first anchor is
    # a link to the CWD's parent. We need to do special processing to get
    # this one correct.
    $self->{first_anchor_processed} = 0;

    # Assume that this is an index unless we find out otherwise (see method
    # `text'.
    $self->{is_index} = 1;

    return $self;
}
    
sub start {
    my $self = shift;

    # If this is not an index then do nothing.
    if (!$self->{is_index}) {
	$self->SUPER::start(@_);
	return;
    }
	
    my($tag, $attr, $attrseq, $origtext) = @_;

    # print "tag: " . $tag . "\n";

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
	# print "Found and anchor! $attr->{href}\n";
	$self->{anchor_seen} = 1;
	$self->{anchor_href} = $attr->{href};
    }

    $self->SUPER::start(@_);
}

sub end {
    my $self = shift;

    # If this is not an index then do nothing.
    if (!$self->{is_index}) {
	$self->SUPER::end(@_);
	return;
    }

    my ($tag, $origtext) = @_;

    # print "tag: " . $tag . "\n";

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
		my ($parent, $junk) = ($self->{base_url} =~ /(.*\/)*(.*\/)/);
		$new_anchor = "<A HREF=" . $self->{dir_cgi} . $parent . ">";
	    }
	    else {
		$new_anchor = "<A HREF=" . $self->{dir_cgi} . 
		    $self->{base_url} . $self->{anchor_href} . ">"; 
	    }
	}
	# Is the href a data file? If so build the URL to the query form.
	# This means that the URL to the dataset must be built from the URL
	# to the data file - the dataset URL goes through a DODS server.
	# Thus, the funny regex match for $machine and $path. I separate the
	# two parts of the data file URL and splice in the DODS server name.
	elsif ($self->{anchor_href} =~ /.*$self->{ext}$/) {
	    my ($machine, $path) = $self->{base_url} =~ /(http:\/\/[^\/]*\/)(.*\/)*/;
#  	    my $data_url = $machine . $self->{server} . $path .
#  		$self->{anchor_href} ;
	    my $data_url = $self->{server} . $path . $self->{anchor_href} ;

	    $new_anchor = "<A HREF=" . $self->{query_cgi} . $data_url . ">"; 
	}
	# Is it a regular file?
	else {
 	    $new_anchor = "<A HREF=" . $self->{anchor_href} . ">"; 
	}

# 	elsif ($self->{anchor_href} =~ /.*$/) {
# 	    $new_anchor = "<A HREF=" . $self->{base_url} .
# 		$self->{anchor_href} . ">"; 
# 	}

	# However we got here, the first anchor has been processed.
	$self->{first_anchor_processed} = 1;

	$self->SUPER::output($new_anchor);
    }
    else {
	$self->SUPER::output(@_);
    }
}
