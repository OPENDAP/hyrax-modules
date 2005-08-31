
# Automake file for dap-server
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news

SUBDIRS = . asciival www-interface

sbin_PROGRAMS = dap_usage

dap_usage_SOURCES = usage.cc

HTML_FILTER = HTML/Filter.pm HTML/LinkExtor.pm HTML/TokeParser.pm \
	HTML/Entities.pm HTML/HeadParser.pm HTML/Parser.pm

# Just a little documentation: 
#
# nobase - preserve the directory structure here (so that the stuff in HTML/
# gets installed in $prefix/share/dap-server/HTML and not
# $prefix/share/dap-server;
#
# dist - include these files in the distribution tar ball;
#
# pkg - create a subdir of $prefix/share with the name of the package; and
# 
# data - put all these in $prefix/share.

nobase_dist_pkgdata_SCRIPTS = DODS_Dispatch.pm DODS_Cache.pm dods_logging.pm \
	FilterDirHTML.pm read_config.pm $(HTML_FILTER) nph-dods

dist_pkgdata_DATA = dap-server.rc EXAMPLE_DODS_STATISTICS
# Make the noinst_ files install into the cgi bin dir if that directory 
# can be determined by configure, otherwise add directions to INSTALL.
#noinst_SCRIPTS = 
#noinst_DATA = 

# don't need to add nph-dods.in or dap-server.rc.in here because they are
# listed in the configure.ac file; automake figures they should be
# distributed.
EXTRA_DIST = EXAMPLE_DODS_STATISTICS README-security COPYING COPYRIGHT_URI

