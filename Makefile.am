
# Automake file for dap-server
#
# 5/17/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf

SUBDIRS = . asciival www-interface

bin_PROGRAMS = dap_usage

dap_usage_SOURCES = usage_main.cc $(USAGE_SOURCES)

if OPENDAPSERVER
    lib_besdir=$(BES_MODULE_DIR)
    lib_bes_LTLIBRARIES = libusage_module.la
    libusage_module_la_SOURCES = $(USAGE_SOURCES) $(BES_SOURCES)
    libusage_module_la_CPPFLAGS = $(BES_CPPFLAGS)
    libusage_module_la_LDFLAGS = -avoid-version -module 
    libusage_module_la_LIBADD = $(BES_LIBS)
endif

USAGE_SOURCES = usage.cc usage.h 

BES_SOURCES = BESUsageModule.h BESUsageNames.h BESUsageResponseHandler.h \
	  BESUsageTransmit.h BESUsageModule.cc BESUsageResponseHandler.cc \
	  BESUsageTransmit.cc BESUsage.h

modulesdir = $(dap_modulesdir)

dist_modules_DATA = DODS_Dispatch.pm DODS_Cache.pm dods_logging.pm \
       FilterDirHTML.pm read_config.pm

cgidir = $(dap_cgidir)
cgiconfdir = $(dap_cgiconfdir)

nodist_cgi_SCRIPTS = nph-dods

nodist_cgiconf_DATA = dap-server.rc

EXTRA_DIST = EXAMPLE_OPENDAP_STATISTICS COPYING COPYRIGHT_URI \
  nph-dods.in dap-server.rc.in dap-server.spec opendap_apache.conf

nph-dods: nph-dods.in config.status
	sed -e 's%[@]PERL[@]%${PERL}%' \
		-e 's%[@]bindir[@]%${bindir}%' \
		-e 's%[@]dap_modulesdir[@]%${dap_modulesdir}%' \
		-e 's%[@]PACKAGE_VERSION[@]%${PACKAGE_VERSION}%' \
		-e 's%[@]dap_cgiconfdir[@]%${dap_cgiconfdir}%' $(srcdir)/nph-dods.in > nph-dods
	chmod +x nph-dods

dap-server.rc: dap-server.rc.in config.status
	sed -e 's%[@]CURL[@]%${CURL}%' \
		-e 's%[@]bindir[@]%${bindir}%' $(srcdir)/dap-server.rc.in > dap-server.rc

DISTCLEANFILES = nph-dods dap-server.rc stamp-nph-dods

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb @PACKAGE@-@PACKAGE_VERSION@.tar.gz
