%define dap_cachedir     /var/cache/dap-server
%define dap_webconfdir   %{_sysconfdir}/httpd/conf.d
%define dap_cgidir       /var/www/opendap

Summary:         Basic request handling for DAP servers 
Name:            dap-server
Version:         3.6.0
Release:         1
License:         GPL
Group:           System Environment/Daemons 
Source0:         ftp://ftp.unidata.ucar.edu/pub/opendap/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   curl libdap-devel >= 3.6.0
Requires:        curl webserver perl

%description
This is base software for our workhorse server. Written using the DAP++ C++ 
library and Perl, this handles processing compressed files and arranging for 
the correct server module to process the file. The base software also 
provides support for the ASCII response and HTML data-request form. Use this 
in combination with one or more of the format-specific handlers.

This package contains all the executable and perl modules. The scripts 
and config files that should be installed in a cgi directory are in the 
documentation directory with -sample appended. If you want a package that 
is usable without manual configuration, install %{name}-cgi.

%package cgi
Summary:        A CGI interface for the DAP server
Requires:       %{name} = %{version}-%{release}
Requires:       freeform_handler netcdf_handler hdf_handler
Group:          System Environment/Daemons

%description cgi
A CGI interface for the DAP server that works without manual configuration.

%prep 
%setup -q

%build
%configure --with-cgidir=%{dap_cgidir}
make %{?_smp_mflags}

# prepend -sample to cgi and config file to install them as doc
cp opendap_apache.conf opendap_apache.conf-sample
cp dap-server.rc dap-server.rc-sample
cp nph-dods nph-dods-sample

# /usr/tmp isn't a safe place, substitute to a dir in 
# /var/cache
sed -e 's:cache_dir /usr/tmp:cache_dir %{dap_cachedir}:' \
   dap-server.rc-sample > dap-server.rc

# cgi-bin dir for the dap-server is in %{ dap_cgidir}, substitute that in
# opendap_apache.conf
sed -e 's:<<prefix>>/share/dap-server-cgi:%{dap_cgidir}:' \
    opendap_apache.conf-sample > opendap_apache.conf

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
install -d -m755 $RPM_BUILD_ROOT/%{dap_cachedir}
install -d -m755 $RPM_BUILD_ROOT/%{dap_webconfdir}
install -m644 opendap_apache.conf $RPM_BUILD_ROOT/%{dap_webconfdir}/opendap_apache.conf

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc COPYING COPYRIGHT_URI EXAMPLE_OPENDAP_STATISTICS NEWS README
# add those as documentation
%doc opendap_apache.conf-sample nph-dods-sample dap-server.rc-sample
%{_bindir}/dap_usage
%{_bindir}/dap_asciival
%{_bindir}/dap_www_int
%{_datadir}/dap-server/
# the webserver must have write access to the cache dir
%attr(-,apache,apache) %{dap_cachedir}

%files cgi
%defattr(-,apache,apache,-)
%config(noreplace) %{dap_webconfdir}/opendap_apache.conf
%dir %{dap_cgidir}
%config(noreplace) %{dap_cgidir}/dap-server.rc
%{dap_cgidir}/nph-dods


%changelog
* Thu Feb  2 2006 Patrice Dumas <dumas@centre-cired.fr> 3.5.3-1
- add a cgi subpackage

* Fri Sep  2 2005 Patrice Dumas <dumas@centre-cired.fr> 3.5.1-1
- initial release
