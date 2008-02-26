
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

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libusage_module.la
besdir=$(datadir)/bes

dist_bin_SCRIPTS = bes-dap-data.sh
dist_bes_DATA = dap-server_help.html dap-server_help.txt

endif

USAGE_SOURCES = usage.cc usage.h 

BES_SOURCES = BESUsageModule.h BESUsageNames.h BESUsageResponseHandler.h \
	  BESUsageTransmit.h BESUsageModule.cc BESUsageResponseHandler.cc \
	  BESUsageTransmit.cc BESUsage.h BESUsageRequestHandler.cc \
	  BESUsageRequestHandler.h

libusage_module_la_SOURCES = $(USAGE_SOURCES) $(BES_SOURCES)
libusage_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libusage_module_la_LDFLAGS = -avoid-version -module 
libusage_module_la_LIBADD = $(BES_DAP_LIBS)

modulesdir = $(dap_modulesdir)

dist_modules_DATA = DODS_Dispatch.pm DODS_Cache.pm dods_logging.pm \
       FilterDirHTML.pm read_config.pm

cgidir = $(dap_cgidir)
cgiconfdir = $(dap_cgiconfdir)

nodist_cgi_SCRIPTS = nph-dods

nodist_cgiconf_DATA = dap-server.rc

EXTRA_DIST = EXAMPLE_OPENDAP_STATISTICS COPYING COPYRIGHT_URI \
  nph-dods.in dap-server.rc.in dap-server.spec opendap_apache.conf \
  README.hyrax README.cgi-server bes-dap-data.sh.in OSX_Resources

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

DISTCLEANFILES = nph-dods dap-server.rc stamp-nph-dods bes-dap-data.sh
CLEANFILES = *~

bes-dap-data.sh: bes-dap-data.sh.in config.status
	$(do_subst) < $(srcdir)/bes-dap-data.sh.in > bes-dap-data.sh
	chmod +x bes-dap-data.sh

if OPENDAPSERVER

CLEANFILES += bes-dap-data.sh

do_subst = sed -e 's,[@]sysconfdir[@],$(sysconfdir),g' \
               -e 's,[@]libdir[@],$(libdir),g' \
               -e 's,[@]datadir[@],$(datadir),g'

# This makes sure that the handler modifies the bes  which confiure found
# even with distcheck is run.
bes-conf: bes-dap-data.sh
	(bes_prefix=`bes-config --prefix` ; \
	if test "$$bes_prefix" = "/usr" ; \
	 then \
	    ./bes-dap-data.sh /etc/bes/bes.conf /usr/lib/bes ;\
	else \
	    ./bes-dap-data.sh $$bes_prefix/etc/bes/bes.conf $$bes_prefix/lib/bes ;\
	fi)

endif

###########################################################################

# Fortify targets.

.PHONY: fortify
fortify:
	sourceanalyzer -b @PACKAGE@ $(MAKE) libusage_module.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr
	(cd asciival && $(MAKE) fortify)
	(cd www-interface && $(MAKE) fortify)

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	sourceanalyzer -b @PACKAGE@ -clean
	(cd asciival && $(MAKE) fortifyclean)
	(cd www-interface && $(MAKE) fortifyclean)

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb @PACKAGE@-@PACKAGE_VERSION@.tar.gz

###########################################################################

# Build OS/X Packages.

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

UNIV_ARCH=-arch ppc -arch i386
MAC_SYSROOT=Developer/SDKs/MacOSX10.4u.sdk

PKG_CONF_FLAGS=

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@

pkg-build:
	./configure --prefix=/usr --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install

pkg-main:
	./OSX_Resources/update_mac_package_contents.pl README.hyrax "configure.ac" "mac_osx/usr"
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg \
		-f mac_osx/usr \
		-ds -r OSX_Resources \
		-i OSX_Resources/Info.plist \
		-d OSX_Resources/Description.plist
	-rm -rf mac_osx

pkg-dmg:
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	mv @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp NEWS @PACKAGE@-@PACKAGE_VERSION@
	cp README.hyrax @PACKAGE@-@PACKAGE_VERSION@/README.hyrax.txt
	cp README.cgi-server @PACKAGE@-@PACKAGE_VERSION@/README.cgi-server.txt
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: clean-pkg pkg-build pkg-main pkg-dmg

