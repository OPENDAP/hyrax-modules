
# Automake file for dap-server
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news

sbin_PROGRAMS = usage

usage_SOURCES = usage.cc

html_filter = HTML/Filter.pm HTML/LinkExtor.pm HTML/TokeParser.pm \
	HTML/Entities.pm HTML/HeadParser.pm HTML/Parser.pm
dist_pkgdata_SCRIPTS = DODS_Dispatch.pm DODS_Cache.pm dods_logging.pm \
	FilterDirHTML.pm read_config.pm $(html_filter)

# Make the noinst_ files install into the cgi bin dir if that directory 
# can be determined by configure, otherwise add directions to INSTALL.
noinst_SCRIPTS = nph-dods
noinst_DATA = dap_server.rc EXAMPLE_DODS_STATISTICS

# don't need to add nph-dods.in or dods.rc.in here because they are listed
# in the configure.ac file; automake figures they should be distributed.
EXTRA_DIST = EXAMPLE_DODS_STATISTICS README-security
