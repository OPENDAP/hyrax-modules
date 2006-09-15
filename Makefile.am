
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

nodist_cgi_SCRIPTS = nph-dods

nodist_cgi_DATA = dap-server.rc

# don't need to add nph-dods.in or dap-server.rc.in here because they are
# listed in the configure.ac file; automake figures they should be
# distributed.
EXTRA_DIST = EXAMPLE_OPENDAP_STATISTICS COPYING COPYRIGHT_URI \
  nph-dods.in dap-server.spec opendap_apache.conf

stamp-nph-dods: nph-dods.in configure
	sed -e "s%[@]PERL[@]%${PERL}%" \
		-e "s%[@]prefix[@]%${prefix}%" \
		-e "s%[@]PACKAGE_VERSION[@]%${PACKAGE_VERSION}%" $< > nph-dods
	chmod +x nph-dods
	touch stamp-nph-dods

DISTCLEANFILES = nph-dods dap-server.rc stamp-nph-dods

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb @PACKAGE@-@PACKAGE_VERSION@.tar.gz
