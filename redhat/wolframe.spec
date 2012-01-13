# Wolframe RPM spec file
#
# Copyright (C) 2011 Project Wolframe

# set distribution based on some OpenSuse and distribution macros
# this is only relevant when building on https://build.opensuse.org
###

%if 0%{?opensuse_bs}

%define rhel 0
%define rhel4 0
%define rhel5 0
%define rhel6 0
%if 0%{?rhel_version} >= 400 && 0%{?rhel_version} <= 499
%define dist rhel4
%define rhel 1
%define rhel4 1
%endif
%if 0%{?rhel_version} >= 500 && 0%{?rhel_version} <= 599
%define dist rhel5
%define rhel 1
%define rhel5 1
%endif
%if 0%{?rhel_version} >= 600 && 0%{?rhel_version} <= 699
%define dist rhel6
%define rhel 1
%define rhel6 1
%endif

%define centos 0
%if 0%{?centos_version} >= 500 && 0%{?centos_version} <= 599
%define dist centos5
%define centos 1
%endif

%if 0%{?centos_version} >= 600 && 0%{?centos_version} <= 699
%define dist centos6
%define centos 1
%endif

%define fedora 0
%define fc14 0
%if 0%{?fedora_version} == 14
%define dist fc14
%define fc14 1
%define fedora 1
%endif
%define fc15 0  
%if 0%{?fedora_version} == 15
%define dist fc15
%define fc15 1
%define fedora 1
%endif
%define fc16 0  
%if 0%{?fedora_version} == 16
%define dist fc16
%define fc16 1   
%define fedora 1
%endif

%define suse 0
%if 0%{?suse_version} == 1140
%define dist osu114
%define suse 1
%endif
%if 0%{?suse_version} > 1140
%define dist osu121
%define suse 1
%endif

%define sles 0
%if 0%{?sles_version} == 11
%define dist sle11
%define sles 1
%endif

%endif

# define what to build
###

%define with_ssl	1
%define with_sqlite	1
%define with_pgsql	1
%define with_lua	1
%define with_pam	1
%define with_sasl	1
%define with_libxml2	1
%define with_libxslt	1
%define with_examples	1

# Qt is far too old on some platforms, we also don't want to build a local
# version here

%define with_qt		1
%if %{rhel} || %{centos}
%define with_qt		0
%endif
%if %{fedora} || %{suse} || %{sles}
%define with_qt		1
%endif

# Boost has sometimes a different layout in the shared libraries, don't
# know why

%define boost_library_tag %{nil}
%if %{fedora}
%define boost_library_tag BOOST_LIBRARY_TAG=
%endif

# build local boost for distributions which have a too old version

%define build_boost 0
%if %{rhel} || %{fc14} || %{centos} || %{sles}
%define build_boost 1
%define boost_version 1.46.1
%define boost_underscore_version 1_46_1
%endif

# init script to start the daemon

%if %{rhel} || %{centos} || %{fedora}
%define initscript	wolframe.initd.RHEL
%endif
%if %{suse} || %{sles}
%define initscript	wolframe.initd.SuSE
%endif

%define WOLFRAME_USR	wolframe
%define WOLFRAME_GRP	wolframe

Summary: Small and medium enterprise resource planning (Wolframe)
Name: wolframe
Version: 0.0.1
Release: 0.2
License: Wolframe License
Group: Application/Business
%if %{build_boost}
Source0: %{name}_%{version}.tar.gz
Source1: boost_%{boost_underscore_version}.tar.gz
%else
Source: %{name}_%{version}.tar.gz
%endif

URL: http://www.wolframe.net/

BuildRoot: %{_tmppath}/%{name}-root

# Build dependencies
###

# our makefile mechanism depends on the Linux release file in '/etc' which
# is provided by many different packages
%if %{rhel}
BuildRequires: redhat-release
%endif
%if %{centos}
BuildRequires: centos-release
%endif
%if %{fedora}
BuildRequires: generic-release
%endif
%if %{suse}
BuildRequires: openSUSE-release
%endif
%if %{sles}
BuildRequires: sles-release
BuildRequires: pwdutils >= 3.2
%endif

%if !%{build_boost}
BuildRequires: boost-devel
%if %{rhel} || %{centos} || %{fedora}
Requires: boost >= 1.43
Requires: boost-thread >= 1.43
Requires: boost-date-time >= 1.43
Requires: boost-filesystem >= 1.43
Requires: boost-program-options >= 1.43
Requires: boost-system >= 1.43
%endif
%if %{suse}
Requires: libboost-thread1_44_0 >= 1.44.0
Requires: libboost-date-time1_44_0 >= 1.44.0
Requires: libboost-filesystem1_44_0 >= 1.44.0
Requires: libboost-program-options1_44_0 >= 1.44.0
Requires: libboost-system1_44_0 >= 1.44.0
%endif
%endif

%if %{with_ssl}
BuildRequires: openssl-devel >= 0.9.7
Requires: openssl >= 0.9.7
%endif
%if %{with_pam}
BuildRequires: pam-devel >= 0.77
Requires: pam >= 0.77
%endif
%if %{with_sasl}
BuildRequires: cyrus-sasl-devel >= 2.1.19
%if %{rhel} || %{centos} || %{fedora}
Requires: cyrus-sasl-lib >= 2.1.19
%endif
%if %{suse} || %{sles}
Requires: cyrus-sasl >= 2.1.22
%endif
%endif
%if %{with_libxml2}
BuildRequires: libxml2-devel >= 2.6
Requires: libxml2 >= 2.6
%endif
%if %{with_libxslt}
BuildRequires: libxslt-devel >= 1.0
Requires: libxslt >= 1.0
%endif
BuildRequires: gcc-c++
BuildRequires: doxygen

# postgres database module
%if %{with_pgsql}
BuildRequires: postgresql-devel >= 7.0
%endif

# build local sqlite3 for distibutions with no or too old version
%define build_sqlite 0   
%if %{with_sqlite}
%if %{rhel}
%if %{rhel4}
%define build_sqlite 1
%endif
%endif
%endif

# sqlite database module
%if %{with_sqlite}
%if %{rhel} || %{centos} || %{fedora}
%if %{rhel}
%if %{rhel5} || %{rhel6}
BuildRequires: sqlite-devel >= 3.0
%endif
%else
BuildRequires: sqlite-devel >= 3.0
%endif
%endif
%if %{suse} || %{sles}
BuildRequires: sqlite3-devel >= 3.0
%endif
%endif

# Check if 'Distribution' is really set by OBS (as mentioned in bacula)
%if ! 0%{?opensuse_bs}
Distribution: %{dist}
%endif

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


%package devel
Summary: Wolframe development files
Group: Application/Business

%description devel
The libraries and header files used for development with Wolframe.

Requires: %{name} >= %{version}-%{release}

%if %{with_pgsql}
%package postgresql
Summary: Wolframe Postgresql database module
Group: Application/Business

%description postgresql
The Wolframe database module for Postgresql (libpq).

Requires: %{name} >= %{version}-%{release}
Requires: postgresql-libs >= 7.0
%endif

%if %{with_sqlite}
%package sqlite3
Summary: Wolframe Sqlite3 database module
Group: Application/Business

%description sqlite3
The Wolframe database module for Sqlite3.

Requires: %{name} >= %{version}-%{release}
%if %{rhel} || %{centos} || %{fedora}
%if %{rhel}
%if %{rhel5} || %{rhel6}
Requires: sqlite >= 3.0
%endif
%else
Requires: sqlite >= 3.0
%endif
%endif
%if %{suse} || %{sles}
Requires: sqlite3 >= 3.0
%endif

%endif

%if %{with_qt}
%package client
Summary: Wolframe client
Group: Application/Business

%if %{rhel} || %{centos} || %{fedora}
BuildRequires: qt4-devel >= 4.5
Requires: qt4 >= 4.5
%endif
%if %{suse} || %{sles}
BuildRequires: libqt4-devel >= 4.5
Requires: libqt4 >= 4.5
%endif

%description client
Qt client for the Wolframe server.

%endif

%prep

%if %{build_boost}
%setup -T -D -b 0 -b 1
%else
%setup
%endif

%build

%if %{build_boost}
cd %{_builddir}/boost_%{boost_underscore_version}
./bootstrap.sh --prefix=/tmp/boost-%{boost_version} \
	--with-libraries=thread,filesystem,system,program_options,date_time
./bjam %{?_smp_mflags} install
%endif

cd %{_builddir}/%{name}-%{version}
LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make help \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%endif
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc libdir=%{_libdir}

LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make config \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%endif
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc libdir=%{_libdir}

LDFLAGS=-Wl,-rpath=%{_libdir}/wolframe make all \
	%{?_smp_mflags} \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%endif
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc libdir=%{_libdir}

cd docs; make doc-doxygen

echo ======================= TESTING ==============================
# make test
echo ===================== END OF TESTING =========================


%install
make DESTDIR=$RPM_BUILD_ROOT install \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%endif
	WITH_SSL=%{with_ssl} WITH_SQLITE3=%{with_sqlite} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT=%{with_qt} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
	WITH_EXAMPLES=%{with_examples} \
	sysconfdir=/etc libdir=%{_libdir}

cd docs && make DESTDIR=$RPM_BUILD_ROOT install && cd ..

%if %{build_boost}
# copy local versions of shared libraries of boost for platforms missing a decent
# version of boost
mkdir -p $RPM_BUILD_ROOT%{_libdir}/wolframe
for i in \
	libboost_program_options.so.%{boost_version} libboost_system.so.%{boost_version} \
	libboost_filesystem.so.%{boost_version} libboost_thread.so.%{boost_version} \
	libboost_date_time.so.%{boost_version}; do
    cp /tmp/boost-%{boost_version}/lib/$i $RPM_BUILD_ROOT%{_libdir}/wolframe/
done
%endif

mkdir -p $RPM_BUILD_ROOT%{_initrddir}
cp redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}


%clean
rm -rf $RPM_BUILD_ROOT


%pre
getent group %{WOLFRAME_GRP} >/dev/null || /usr/sbin/groupadd %{WOLFRAME_GRP}
%if %{rhel} || %{centos} || %{fedora}
getent passwd %{WOLFRAME_USR} >/dev/null || /usr/sbin/useradd -g %{WOLFRAME_GRP} %{WOLFRAME_USR} -c "Wolframe user" -d /dev/null
%endif
%if %{suse} || %{sles}
getent passwd %{WOLFRAME_USR} >/dev/null || /usr/sbin/useradd -g %{WOLFRAME_GRP} %{WOLFRAME_USR} -c "Wolframe user"
%endif
 
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

%if %{build_boost}
%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libboost_program_options.so.%{boost_version}
%{_libdir}/wolframe/libboost_system.so.%{boost_version}
%{_libdir}/wolframe/libboost_filesystem.so.%{boost_version}
%{_libdir}/wolframe/libboost_thread.so.%{boost_version}
%{_libdir}/wolframe/libboost_date_time.so.%{boost_version}
%endif

%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libwolframe.so.0.0.0
%{_libdir}/wolframe/libwolframe.so.0

%dir %{_libdir}/wolframe/modules

%{_libdir}/wolframe/modules/mod_audit_textfile.so
%{_libdir}/wolframe/modules/mod_audit_database.so

%{_libdir}/wolframe/modules/mod_auth_textfile.so
%{_libdir}/wolframe/modules/mod_auth_database.so

%{_libdir}/wolframe/modules/mod_authz_database.so

%{_libdir}/wolframe/modules/mod_proc_echo.so


#%dir %{_datadir}/wolframe
#%doc LICENSE


%files doc
%defattr( -, root, root )
%dir %{_datadir}/doc/wolframe
%{_datadir}/doc/wolframe/html

%files devel
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libwolframe.so
%{_libdir}/wolframe/libwolframe.a
%dir %{_includedir}/wolframe
%{_includedir}/wolframe/*.hpp
%dir %{_includedir}/wolframe/config/
%{_includedir}/wolframe/config/*.hpp
%dir %{_includedir}/wolframe/AAAA/
%{_includedir}/wolframe/AAAA/*.hpp
%dir %{_includedir}/wolframe/processor/
%{_includedir}/wolframe/processor/*.hpp
%dir %{_includedir}/wolframe/logger/
%{_includedir}/wolframe/logger/*.hpp
%dir %{_includedir}/wolframe/database/
%{_includedir}/wolframe/database/*.hpp
%dir %{_includedir}/wolframe/protocol/
%{_includedir}/wolframe/protocol/*.hpp

%if %{with_pgsql}
%files postgresql
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_db_postgresql.so
%endif

%if %{with_sqlite}
%files sqlite3
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_db_sqlite3.so
%endif

%if %{with_qt}
%files client
%defattr( -, root, root )
# funny, why?!
%if !%{sles}
%dir %{_bindir}
%endif
%{_bindir}/qtclient
%endif

%changelog
* Sun Aug 29 2011 Andreas Baumann <abaumann@yahoo.com> 0.0.1-0.2
- more splitting into sub-packages for modules
- builds on OpenSuse Build Service (osc)

* Sun Aug 29 2010 Mihai Barbos <mihai.barbos@gmail.com> 0.0.1-0.1
- preliminary release
