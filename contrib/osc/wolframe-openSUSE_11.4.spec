%define dist		osu114

%define initscript	wolframe.initd.SuSE

%define WOLFRAME_USR	wolframe
%define WOLFRAME_GRP	wolframe

%define with_ssl	1
%define with_sqlite	1
%define with_pgsql	1
%define with_lua	1
%define with_pam	1
%define with_sasl	1
%define with_qt		1
%define with_examples	1
 
Summary: Small and medium enterprise resource planning (Wolframe)
Name: wolframe
Version: 0.0.1
Release: 0.1
License: Wolframe License
Group: Application/Business
Source: %{name}-%{version}.tar.bz2

URL: http://www.wolframe.net/

BuildRoot: %{_tmppath}/%{name}-root
BuildRequires: openSUSE-release
BuildRequires: boost-devel >= 1.44.0
Requires: boost >= 1.44.0
Requires: libboost-thread1_44_0 >= 1.44.0
Requires: libboost-date-time1_44_0 >= 1.44.0
Requires: libboost-filesystem1_44_0 >= 1.44.0
Requires: libboost-program-options1_44_0 >= 1.44.0
Requires: libboost-system1_44_0 >= 1.44.0
%if %{with_ssl}
BuildRequires: openssl-devel >= 0.9.8
Requires: openssl >= 0.9.8
%endif
%if %{with_sqlite}
BuildRequires: sqlite3-devel >= 3.0
Requires: sqlite3 >= 3.0
%endif
%if %{with_pgsql}
BuildRequires: postgresql-devel >= 8.1
Requires: postgresql-libs >= 8.1
%endif
%if %{with_pam}
BuildRequires: pam-devel >= 0.99
Requires: pam >= 0.99
%endif
%if %{with_sasl}
BuildRequires: cyrus-sasl-devel >= 2.1.23
Requires: cyrus-sasl >= 2.1.23
%endif
BuildRequires: gcc-c++
BuildRequires: doxygen

Distribution: OpenSuSE 11.4
Vendor: Wolframe team
Packager: Mihai Barbos <mihai.barbos@gmail.com>

%description
The Wolframe server and server utilities


%package doc
Summary: Wolframe documentation
Group: Application/Business

%description doc
The Wolframe documentation.
This package contains the one file html documentation,
multiple file html documentation and the pdf documentation.


%if %{with_qt}
%package client
Summary: Wolframe client
Group: Application/Business

BuildRequires: libqt4-devel >= 4.5
Requires: libqt4 >= 4.5

%description client
Qt client for the Wolframe server.

%endif

%prep
%setup


%build
LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make help \
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc
LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make config \
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc
LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make all \
	%{?_smp_mflags} \
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc
cd docs; make doc-doxygen

echo ======================= TESTING ==============================
# make test
echo ===================== END OF TESTING =========================


%install
make DESTDIR=$RPM_BUILD_ROOT install \
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=$RPM_BUILD_ROOT/etc
cd docs && make DESTDIR=$RPM_BUILD_ROOT install && cd ..

mkdir -p $RPM_BUILD_ROOT%{_libdir}/wolframe

mkdir -p $RPM_BUILD_ROOT%{_initrddir}
cp redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}


%clean
rm -rf $RPM_BUILD_ROOT


%pre
getent group %{WOLFRAME_GRP} >/dev/null || /usr/sbin/groupadd %{WOLFRAME_GRP}
getent passwd %{WOLFRAME_USR} >/dev/null || /usr/sbin/useradd -g %{WOLFRAME_GRP} %{WOLFRAME_USR} -c "Wolframe user" -d /dev/null

# Don't enable Wolframe server at install time, just inform root how this is done
echo
echo Use '/sbin/chkconfig --add wolframed' and '/sbin/ckconfig wolframed on' to enable the
echo Wolframe server at startup
echo


%preun
if [ "$1" = 0 ]; then
    /etc/init.d/wolframe stop > /dev/null 2>&1
    /sbin/chkconfig --del wolframe
fi


%postun
if [ "$1" = 0 ]; then
    /usr/sbin/userdel %{WOLFRAME_USR}
fi

%files
%defattr( -, root, root )
%attr( 554, root, root) %{_initrddir}/%{name}
%{_sbindir}/wolframed
%dir %attr(0755, root, root) %{_sysconfdir}/wolframe
%config %attr(0755, root, root) %{_sysconfdir}/wolframe/wolframe.conf
#%attr(0755, WOLFRAME_USR, WOLFRAME_GRP) %dir /var/log/wolframe
#%attr(0755, WOLFRAME_USR, WOLFRAME_GRP) %dir /var/run/wolframe

%dir %{_libdir}/wolframe

#%dir %{_datadir}/wolframe
#%doc LICENSE


%files doc
%defattr( -, root, root )
%dir %{_datadir}/doc/wolframe
%{_datadir}/doc/wolframe/html

%if %{with_qt}
%files client
%defattr( -, root, root )
%dir %{_bindir}
%{_bindir}/qtclient
#%dir %{_libdir}/wolframe
#%{_libdir}/wolframe/libQtCore.so.4
#%{_libdir}/wolframe/libQtGui.so.4
#%{_libdir}/wolframe/libQtNetwork.so.4
#%{_libdir}/wolframe/libQtXml.so.4
%endif

%changelog
* Sun Aug 29 2010 Mihai Barbos <mihai.barbos@gmail.com> 0.9.0-0.1
- preliminary release

