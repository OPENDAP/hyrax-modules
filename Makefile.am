
# Automake file for dap-server
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf

SUBDIRS = . asciival www-interface

bin_PROGRAMS = dap_usage

dap_usage_SOURCES = usage.cc

modulesdir = $(dap_modulesdir)

dist_modules_DATA = DODS_Dispatch.pm DODS_Cache.pm dods_logging.pm \
       FilterDirHTML.pm read_config.pm

cgidir = $(dap_cgidir)

nodist_cgi_SCRIPTS = nph-dods

nodist_cgi_DATA = dap-server.rc

# don't need to add nph-dods.in or dap-server.rc.in here because they are
# listed in the configure.ac file; automake figures they should be
# distributed.
EXTRA_DIST = EXAMPLE_DODS_STATISTICS README-security COPYING COPYRIGHT_URI \
  README.ascii README.www_int nph-dods.in dap-server.spec

nph-dods: nph-dods.in configure
	sed -e "s%[@]PERL[@]%${PERL}%" \
		-e "s%[@]prefix[@]%${prefix}%" \
		-e "s%[@]bindir[@]%${bindir}%" \
		-e "s%[@]modulesdir[@]%${modulesdir}%" \
		-e "s%[@]PACKAGE_VERSION[@]%${PACKAGE_VERSION}%" $< > nph-dods
	chmod +x nph-dods

DISTCLEANFILES = nph-dods dap-server.rc
