
# Send DODS loggin information to stdout. This will be run in response to a
# status request to the server dispatch code (nph-dods).

package dods_logging;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(print_log_info);

use strict;

sub print_log_info {
    my $access_log_location=shift;
    my $error_log_location=shift;

    print "Access information:\n";
    open ACCESS, $access_log_location 
	or die "Could not open the access log file ($access_log_location)\n";
    while(<ACCESS>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "Error information:\n";
    open ERROR, $error_log_location 
	or die "Could not open the error log file ($error_log_location)\n";
    while(<ERROR>) {
	if (m/nph-dods/) {
	    print;
	}
    }

    print "End of log information\n";
}

# &print_log_info("/var/log/httpd/access_log", "/var/log/httpd/error_log");

# $Log: dods_logging.pm,v $
# Revision 1.2  2002/12/31 22:26:45  jimg
# Added.
#
