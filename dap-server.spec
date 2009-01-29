%define dap_cachedir     /var/cache/dap-server
%define dap_webconfdir   %{_sysconfdir}/httpd/conf.d
%define dap_cgidir       %{_datadir}/opendap-cgi
%define dap_cgiconfdir   %{_sysconfdir}/opendap/
%define __perl_provides %{nil}
%define __perl_requires %{nil}

Summary:         Basic request handling for OPeNDAP servers 
Name:            dap-server
Version:         3.9.0
Release:         1
License:         LGPL
Group:           System Environment/Daemons 
Source0:         http://www.opendap.org/pub/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   libdap-devel >= 3.9.0
BuildRequires:   bes-devel >= 3.7.0
# we use httpd and not webserver because we make use of the apache user. 
Requires:        curl httpd
Requires:        perl perl(HTML::Filter) perl(Time::Local) perl(POSIX)
Requires:        perl(LWP::Simple)

%description
This is base software for the CGI-based OPeNDAP (Open-source Project for 
a Network Data Access Protocol) server. Written using the DAP++ C++ 
library and Perl, this handles processing compressed files and arranging 
for the correct server module to process the file. The base software also 
provides support for the ASCII response and HTML data-request form. Use 
this in combination with one or more of the format-specific handlers.

This package contains all the executable and perl modules. The scripts 
and config files that should be installed in a cgi directory are in the 
documentation directory with -sample appended. If you want a package that 
is usable without manual configuration, install %{name}-cgi.

%package cgi
Summary:        A preconfigured CGI interface for the OPeNDAP server
Requires:       %{name} = %{version}-%{release}
Requires:       freeform_handler netcdf_handler hdf4_handler
Group:          System Environment/Daemons

%description cgi
A CGI interface for the OPeNDAP server that works without manual 
configuration. The web server is setup such that the CGI directory is
at %{dap_cgidir}. 

The config file for the cgi script, dap-server.rc, is in 
%{dap_cgiconfdir}.

The default configuration allows for the use of the following handlers:
freeform, netcdf and hdf4.

The dataset directory is to be set in dap-server.rc, it defaults to the 
web server DocumentRoot.

%prep 
%setup -q

%build
%configure --with-cgidir=%{dap_cgidir} --disable-dependency-tracking \
 --with-cgiconfdir=%{dap_cgiconfdir} --disable-static
make %{?_smp_mflags}

# keep the nph-dods timestamp
touch -r nph-dods __nph-dods_stamp

# prepend -sample to cgi and config file to install them as doc
rm -rf __dist_doc
mkdir __dist_doc
cp -p opendap_apache.conf __dist_doc/opendap_apache.conf-sample
cp -p dap-server.rc __dist_doc/dap-server.rc-sample
cp -p nph-dods __dist_doc/nph-dods-sample
chmod a-x __dist_doc/nph-dods-sample

# adjust jgofs paths (even though jgofs isn't used, it is more consistent)
sed -i -e 's:^\$ENV{"JGOFS_METHOD"} = "`pwd`";:\$ENV{"JGOFS_METHOD"} = "%{_bindir}";:' nph-dods
sed -i -e 's:^\$ENV{"JGOFS_OBJECT"} = "`pwd`";:\$ENV{"JGOFS_OBJEXT"} = "%{dap_cgidir}";:' nph-dods

# /usr/tmp isn't a safe place, substitute to a dir in 
# /var/cache
sed -e 's:cache_dir /usr/tmp:cache_dir %{dap_cachedir}:' \
   __dist_doc/dap-server.rc-sample > dap-server.rc
touch -r __dist_doc/dap-server.rc-sample dap-server.rc
touch -r __nph-dods_stamp nph-dods
rm __nph-dods_stamp

# cgi-bin dir for the dap-server is in %%{dap_cgidir}, substitute that in
# opendap_apache.conf
sed -e 's:<<prefix>>/share/dap-server-cgi:%{dap_cgidir}:' \
    __dist_doc/opendap_apache.conf-sample > opendap_apache.conf._distributed
touch -r __dist_doc/opendap_apache.conf-sample opendap_apache.conf._distributed

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install INSTALL='install -p'
install -d -m755 $RPM_BUILD_ROOT%{dap_cachedir}
install -d -m755 $RPM_BUILD_ROOT%{dap_webconfdir}
install -d -m755 $RPM_BUILD_ROOT%{dap_cgiconfdir}
install -p -m644 opendap_apache.conf._distributed $RPM_BUILD_ROOT%{dap_webconfdir}/opendap_apache.conf

rm $RPM_BUILD_ROOT%{_libdir}/bes/*.la

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc COPYING COPYRIGHT_URI EXAMPLE_OPENDAP_STATISTICS NEWS README
%doc __dist_doc/*
%{_bindir}/bes-dap-data.sh
%{_bindir}/dap_usage
%{_bindir}/dap_asciival
%{_bindir}/dap_www_int
%{_datadir}/bes/
%{_datadir}/dap-server/
%{_libdir}/bes/libascii_module.so
%{_libdir}/bes/libusage_module.so
%{_libdir}/bes/libwww_module.so
# the webserver must have write access to the cache dir
%attr(-,apache,apache) %{dap_cachedir}

%files cgi
%defattr(-,root,root,-)
%config(noreplace) %{dap_webconfdir}/opendap_apache.conf
%dir %{dap_cgiconfdir}
%config(noreplace) %{dap_cgiconfdir}/dap-server.rc
%{dap_cgidir}/


%changelog
* Thu Jan 29 2009 James Gallagher <jimg@zoe.opendap.org> - 3.9.0-1
- Updated

* Mon Mar 3 2008 Patrick West <patrick@ucar.edu> 3.8.5-1
- Update for 3.8.5; General fixes

* Wed Dec 3 2007 James Gallagher <jgallagher@opendap.org> 3.7.3-1
- Update for 3.8.4; General fixes

* Wed Nov 14 2007 James Gallagher <jgallagher@opendap.org> 3.7.3-1
- Update for 3.8.3; General fixes

* Wed Feb 14 2007 James Gallagher <jgallagher@opendap.org> 3.7.3-1
- Update for 3.7.3; includes adding BES modules

* Wed Sep 20 2006 Patrice Dumas <dumas@centre-cired.fr> 3.7.1-1
- update to 3.7.1

* Fri Mar  3 2006 Patrice Dumas <dumas@centre-cired.fr> 3.6.0-1
- new release

* Wed Feb 22 2006 Ignacio Vazquez-Abrams <ivazquez@ivazquez.net> 3.5.3-1.2
- Small fix for Perl provides

* Thu Feb  2 2006 Patrice Dumas <dumas@centre-cired.fr> 3.5.3-1
- add a cgi subpackage

* Fri Sep  2 2005 Patrice Dumas <dumas@centre-cired.fr> 3.5.1-1
- initial release
