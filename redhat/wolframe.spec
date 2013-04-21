# Wolframe RPM spec file
#
# Copyright (C) 2011-2013 Project Wolframe

# Set distribution based on some OpenSuse and distribution macros
# this is only relevant when building on https://build.opensuse.org
###

%define rhel 0
%define rhel5 0
%define rhel6 0
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
%define centos5 0
%if 0%{?centos_version} >= 500 && 0%{?centos_version} <= 599
%define dist centos5
%define centos 1
%define centos5 1
%endif

%define centos6 0
%if 0%{?centos_version} >= 600 && 0%{?centos_version} <= 699
%define dist centos6
%define centos 1
%define centos6 1
%endif

%define fedora 0
%define fc17 0
%if 0%{?fedora_version} == 17
%define dist fc17
%define fc17 1
%define fedora 1
%endif
%define fc18 0
%if 0%{?fedora_version} == 18
%define dist fc18
%define fc18 1
%define fedora 1
%endif

%define suse 0
%define osu114 0
%define osu121 0
%define osu122 0
%define osu131 0
%if 0%{?suse_version} == 1140
%define dist osu114
%define osu114 1
%define suse 1
%endif
%if 0%{?suse_version} == 1210
%define dist osu121
%define osu121 1
%define suse 1
%endif
%if 0%{?suse_version} >= 1220
%define dist osu122
%define osu122 1
%define suse 1
%endif
%if 0%{?suse_version} >= 1310
%define dist osu131
%define osu122 1
%define suse 1
%endif

%define sles 0
%if 0%{?sles_version} == 11
%define dist sle11
%define sles 1
%endif

# Define what to build
###

%define with_ssl	1
%define with_sqlite	1
%define with_pgsql	1
%define with_lua	1
%define with_pam	1
%define with_sasl	1
%define with_libxml2	1
%define with_libxslt	1
%define with_libhpdf	1
%define with_freeimage	1
%define with_examples	1

# Qt is far too old on some platforms, we also don't want to build a local
# version here

%define with_qt4	1
%if %{rhel} || %{centos}
%define with_qt4	0
%endif
%if %{fedora} || %{suse} || %{sles}
%define with_qt4	1
%endif

# Per package decisions
###

# Boost has sometimes a different layout in the shared libraries, don't
# know why

%define boost_library_tag %{nil}
%if %{fedora}
%define boost_library_tag BOOST_LIBRARY_TAG=
%endif

# build local boost for distributions which have a too old version

%define build_boost 0
%if %{rhel} || %{centos} || %{sles}
%define build_boost 1
%define boost_version 1.48.0
%define boost_underscore_version 1_48_0
%endif
%if %{suse}
%if %{osu114} || %{osu121}
%define build_boost 1
%define boost_version 1.48.0
%define boost_underscore_version 1_48_0
%endif
%endif

# icu for boost-locale is available natively only on a few platforms,
# enable it there. If we build our own boost, try hard to enable ICU
# everywhere
%define with_icu	0
%if !%{build_boost}
%define with_icu	0
%if %{fedora}
%if %{fc17} || %{fc18}
%define with_icu	1
%endif
%endif
%if %{suse}
%if %{osu122} || %{osu131}
%define with_icu	1
%endif
%endif
%endif

%if %{build_boost}
%define with_icu	1
%if %{rhel}
%if %{rhel6}
# No icu-devel on RHEL6 on OSC due to license issues from Redhat!
%define with_icu	0
%endif
%endif
%endif

# Build local libxml2 for distributions which have a too old broken version
# (broken in respect to some character encodings, not broken as such)

%if %{with_libxml2}
%define build_libxml2 0
%if %{rhel}
%if %{rhel5}
%define build_libxml2 1
%define libxml2_version 2.7.8
%endif
%endif
%if %{centos}
%if %{centos5}
%define build_libxml2 1
%define libxml2_version 2.7.8
%endif
%endif
%endif

# init script to start the daemon

%if %{rhel} || %{centos} || %{fedora}
%define initscript	wolframed.initd.RHEL
%endif
%if %{suse} || %{sles}
%define initscript	wolframed.initd.SuSE
%endif

%define configuration	wolframe.conf

%define systemctl_configuration wolframed.service

%define firewalld_configuration wolframe-firewalld.xml

%define WOLFRAME_USR	wolframe
%define WOLFRAME_GRP	wolframe

Summary: Small and medium enterprise resource planning (Wolframe)
Name: wolframe
Version: 0.0.1
Release: 0.2
License: Wolframe License
Group: Application/Business
Source: %{name}_%{version}.tar.gz
%if %{build_boost}
Source1: boost_%{boost_underscore_version}.tar.gz
Patch0: boost_%{boost_underscore_version}-gcc-compile.patch
%endif
%if %{build_libxml2}
Source2: libxml2-sources-%{libxml2_version}.tar.gz
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
%if %{fedora} && !0%{?opensuse_bs}
BuildRequires: fedora-release
%endif
%if %{fedora} && 0%{?opensuse_bs}
BuildRequires: generic-release
%endif
%if %{suse}
BuildRequires: openSUSE-release
%endif
%if %{sles}
BuildRequires: sles-release
BuildRequires: pwdutils >= 3.2
%endif

%if %{fedora}
%if %{fc17}
BuildRequires: systemd-units
%endif
%if %{fc18}
BuildRequires: systemd
%endif
%endif
%if %{suse}
%if %{osu122} || %{osu131}
BuildRequires: systemd
%{?systemd_requires}
%endif
%endif

%if %{build_boost}
%if %{with_icu}
%if %{centos} || %{fedora}
BuildRequires: libicu-devel >= 3.6
%endif
%if %{rhel}
%if !%{rhel6}
# see http://permalink.gmane.org/gmane.linux.suse.opensuse.buildservice/17779
BuildRequires: libicu-devel >= 3.6
%endif
%endif
%if %{suse} || %{sles}
BuildRequires: libicu-devel >= 4.0
%endif
%endif
%else
BuildRequires: boost-devel
%if %{rhel} || %{centos} || %{fedora}
Requires: boost >= 1.48
Requires: boost-thread >= 1.48
Requires: boost-date-time >= 1.48
Requires: boost-filesystem >= 1.48
Requires: boost-program-options >= 1.48
Requires: boost-system >= 1.48
Requires: boost-locale >= 1.48
Requires: boost-regex >= 1.48
%endif
%if %{suse}
%if %{osu122} || %{osu131}
Requires: libboost_thread1_49_0 >= 1.49.0
Requires: libboost_date_time1_49_0 >= 1.49.0
Requires: libboost_filesystem1_49_0 >= 1.49.0
Requires: libboost_program_options1_49_0 >= 1.49.0
Requires: libboost_system1_49_0 >= 1.49.0
Requires: libboost_regex1_49_0 >= 1.49.0
%endif
%endif
%endif

%if %{with_ssl}
BuildRequires: openssl-devel >= 0.9.7
Requires: openssl >= 0.9.7
%endif

%if %{with_pam}
BuildRequires: pam-devel >= 0.77
%endif

%if %{with_sasl}
BuildRequires: cyrus-sasl-devel >= 2.1.19
%endif

%if %{with_libxml2}
%if !%{build_libxml2}
BuildRequires: libxml2-devel >= 2.6
%endif
%endif

%if %{with_libxslt}
BuildRequires: libxslt-devel >= 1.0
%endif

BuildRequires: gcc-c++
BuildRequires: doxygen

# postgres database module
%if %{with_pgsql}
%if %{rhel} 
%if %{rhel5}
BuildRequires: postgresql84-devel >= 8.4
%else
BuildRequires: postgresql-devel >= 8.3
%endif
%endif
%if %{centos}
%if %{centos5}
BuildRequires: postgresql84-devel >= 8.4
%else
BuildRequires: postgresql-devel >= 8.3
%endif
%endif
%if %{fedora} || %{suse} || %{sles}
BuildRequires: postgresql-devel >= 8.3
%endif
%endif

# build local sqlite3 for distibutions with no or too old version
# or which do not support V2 of the API correctly or which have
# been compiled without threading support

%define build_sqlite 0   
%if %{with_sqlite}
%if %{rhel}
%if %{rhel5}
%define build_sqlite 1
%endif
%endif
%if %{centos}
%if %{centos5}
%define build_sqlite 1
%endif
%endif
%endif

# if we use the system one, we must pick the right version
%if !%{build_sqlite}
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
%endif

# build local version of libharu/libpdf
%define build_libhpdf 1
%if %{with_libhpdf}
%if %{fedora}
%if %{fc18}
%define build_libhpdf 0
%endif
%endif
%endif

%if !%{build_libhpdf}
%if %{with_libhpdf}
BuildRequires: libharu-devel >= 2.2.1
%endif
%else
# building libhpdf requires zlib and libpng development libraries
BuildRequires: libpng-devel
BuildRequires: zlib-devel
%endif

# FreeImage, build or use local version
%define build_freeimage 1

%if !%{build_freeimage}
%if %{with_freeimage}
BuildRequires: freeimage-devel >= 3.15.4
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

%if %{with_pam}
%package pam
Summary: Wolframe PAM authentication module
Group: Application/Business

%description pam
The Wolframe authentication module for PAM.

Requires: %{name} >= %{version}-%{release}
Requires: pam >= 0.77
%endif

%if %{with_sasl}
%package sasl
Summary: Wolframe SASL authentication module
Group: Application/Business

%description sasl
The Wolframe authentication module using Cyrus SASL.

Requires: %{name} >= %{version}-%{release}
%if %{rhel} || %{centos} || %{fedora}
Requires: cyrus-sasl-lib >= 2.1.19
%endif
%if %{suse} || %{sles}
Requires: cyrus-sasl >= 2.1.22
%endif

%endif

%if %{with_libxml2}
%package libxml2
Summary: Wolframe XML filtering module using libxml2
Group: Application/Business

%description libxml2
The Wolframe XML parsing module using libxml2.

Requires: %{name} >= %{version}-%{release}
%if !%{build_libxml2}
Requires: libxml2 >= 2.6
%endif
%endif

%if %{with_libxslt}
%package libxslt
Summary: The Wolframe filter module using libxslt
Group: Application/Business

%description libxslt
The Wolframe filter module using libxslt.

Requires: %{name} >= %{version}-%{release}
Requires: libxslt >= 1.0
%endif

%if %{with_libhpdf}
%package libhpdf
Summary: Wolframe printing module based on libhpdf/libharu
Group: Application/Business

%description libhpdf
Wolframe printing module based on libhpdf/libharu.

Requires: %{name} >= %{version}-%{release}
Requires: libpng
Requires: zlib
%endif

%if %{with_freeimage}
%package freeimage
Summary: Wolframe image manipulation function module
Group: Application/Business

%description freeimage
Wolframe image manipulation function module implemented with
the FreeImage library.

Requires: %{name} >= %{version}-%{release}
%endif

%package libclient
Summary: Wolframe C++ client library
Group: Application/Business

%description libclient
Wolframe client library for C++ (implemented using boost libraries).

%if !%{build_boost}
BuildRequires: boost-devel
%if %{rhel} || %{centos} || %{fedora}
Requires: boost >= 1.48
Requires: boost-thread >= 1.48
Requires: boost-date-time >= 1.48
Requires: boost-filesystem >= 1.48
Requires: boost-program-options >= 1.48
Requires: boost-system >= 1.48
%endif
%if %{suse}
%if %{osu122} || %{osu131}
Requires: libboost_thread1_49_0 >= 1.49.0
Requires: libboost_date_time1_49_0 >= 1.49.0
Requires: libboost_filesystem1_49_0 >= 1.49.0
Requires: libboost_program_options1_49_0 >= 1.49.0
Requires: libboost_system1_49_0 >= 1.49.0
%endif
%endif
%endif

%if %{with_ssl}
Requires: openssl >= 0.9.7
%endif

%package libclient-devel
Summary: Development header files and libraries for the Wolframe C++ client library
Group: Application/Business
Requires: %{name}-libclient >= %{version}-%{release}

%description libclient-devel
Development files for the Wolframe client library for C++ (implemented using boost libraries).

%package client
Summary: Wolframe client command line tool
Group: Application/Business
Requires: %{name}-libclient >= %{version}-%{release}

%description client
Command line client to access the Wolframe server.

%if %{with_qt4}
%package qtclient
Summary: Wolframe Qt frontend
Group: Application/Business

%if %{rhel} || %{centos} || %{fedora}
BuildRequires: qt4-devel >= 4.5
Requires: qt4 >= 4.5
%endif
%if %{suse} || %{sles}
BuildRequires: libqt4-devel >= 4.5
Requires: libqt4 >= 4.5
%endif

%description qtclient
Qt client for the Wolframe server.

%endif

%prep

%if %{build_boost} && %{build_libxml2}
%setup -T -D -b 0 -b 1 -b 2
cd ../boost_%{boost_underscore_version}
%patch -P 0 -p1
cd ../%{name}-%{version}
%else
%if %{build_boost}
%setup -T -D -b 0 -b 1
cd ../boost_%{boost_underscore_version}
%patch -P 0 -p1
cd ../%{name}-%{version}
%else
%setup
%endif
%endif

%build

%if %{build_boost}
cd %{_builddir}/boost_%{boost_underscore_version}
./bootstrap.sh --prefix=/tmp/boost-%{boost_version} \
	--with-libraries=thread,filesystem,system,program_options,date_time,regex,locale
./bjam %{?_smp_mflags} -d1 install
%endif

%if %{build_libxml2}
cd %{_builddir}/libxml2-%{libxml2_version}
./configure --prefix=/tmp/libxml2-%{libxml2_version}
make %{?_smp_mflags}
make install
%endif

cd %{_builddir}/%{name}-%{version}
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe -Wl,-rpath=%{_libdir}/wolframe/plugins" make help \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=%{with_sqlite} \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT4=%{with_qt4} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
%if %{build_libhpdf}
	WITH_LOCAL_LIBHPDF=1 \
%else
	WITH_SYSTEM_LIBHPDF=%{with_libhpdf} \
%endif
	WITH_ICU=%{with_icu} WITH_EXAMPLES=%{with_examples} \
%if %{build_freeimage}
	WITH_LOCAL_FREEIMAGE=1 \
%else
	WITH_SYSTEM_FREEIMAGE=%{with_freeimage} \
%endif
	sysconfdir=/etc libdir=%{_libdir}

LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe -Wl,-rpath=%{_libdir}/wolframe/plugins" make config \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT4=%{with_qt4} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
%if %{build_libhpdf}
	WITH_LOCAL_LIBHPDF=1 \
%else
	WITH_SYSTEM_LIBHPDF=%{with_libhpdf} \
%endif
	WITH_ICU=%{with_icu} WITH_EXAMPLES=%{with_examples} \
%if %{build_freeimage}
	WITH_LOCAL_FREEIMAGE=1 \
%else
	WITH_SYSTEM_FREEIMAGE=%{with_freeimage} \
%endif
	sysconfdir=/etc libdir=%{_libdir}

LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe -Wl,-rpath=%{_libdir}/wolframe/plugins" make depend \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT4=%{with_qt4} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
%if %{build_libhpdf}
	WITH_LOCAL_LIBHPDF=1 \
%else
	WITH_SYSTEM_LIBHPDF=%{with_libhpdf} \
%endif
	WITH_ICU=%{with_icu} WITH_EXAMPLES=%{with_examples} \
%if %{build_freeimage}
	WITH_LOCAL_FREEIMAGE=1 \
%else
	WITH_SYSTEM_FREEIMAGE=%{with_freeimage} \
%endif
	sysconfdir=/etc libdir=%{_libdir}

LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe -Wl,-rpath=%{_libdir}/wolframe/plugins" make all \
	%{?_smp_mflags} \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT4=%{with_qt4} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
%if %{build_libhpdf}
	WITH_LOCAL_LIBHPDF=1 \
%else
	WITH_SYSTEM_LIBHPDF=%{with_libhpdf} \
%endif
	WITH_ICU=%{with_icu} WITH_EXAMPLES=%{with_examples} \
%if %{build_freeimage}
	WITH_LOCAL_FREEIMAGE=1 \
%else
	WITH_SYSTEM_FREEIMAGE=%{with_freeimage} \
%endif
	sysconfdir=/etc libdir=%{_libdir}

cd docs; make doc-doxygen

echo ======================= TESTING ==============================
# make test
echo ===================== END OF TESTING =========================


%install
make DESTDIR=$RPM_BUILD_ROOT install \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_QT4=%{with_qt4} WITH_LIBXML2=%{with_libxml2} \
	WITH_LIBXSLT=%{with_libxslt} \
%if %{build_libhpdf}
	WITH_LOCAL_LIBHPDF=1 \
%else
	WITH_SYSTEM_LIBHPDF=%{with_libhpdf} \
%endif
	WITH_ICU=%{with_icu} WITH_EXAMPLES=%{with_examples} \
%if %{build_freeimage}
	WITH_LOCAL_FREEIMAGE=1 \
%else
	WITH_SYSTEM_FREEIMAGE=%{with_freeimage} \
%endif
	sysconfdir=/etc libdir=%{_libdir}

cd docs && make DESTDIR=$RPM_BUILD_ROOT install && cd ..

# copy local versions of shared libraries of boost for platforms missing a decent
# version of boost (RHEL, for Debian like systems there is an OSC boost package)
%if %{build_boost}
for i in \
	libboost_program_options.so.%{boost_version} libboost_system.so.%{boost_version} \
	libboost_filesystem.so.%{boost_version} libboost_thread.so.%{boost_version} \
	libboost_date_time.so.%{boost_version} libboost_locale.so.%{boost_version} \
	libboost_regex.so.%{boost_version}; do
	cp /tmp/boost-%{boost_version}/lib/$i $RPM_BUILD_ROOT%{_libdir}/wolframe/
done
%endif

# copy a decent version of libxml2 to local library directory for platforms
# which need it
%if %{build_libxml2}
cp /tmp/libxml2-%{libxml2_version}/lib/libxml2.so.%{libxml2_version} $RPM_BUILD_ROOT%{_libdir}/wolframe/
ln -s libxml2.so.%{libxml2_version} $RPM_BUILD_ROOT%{_libdir}/wolframe/libxml2.so.2
%endif

%if %{rhel} || %{centos} || %{sles}
install -D -m775 redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}d
%endif
%if %{suse}
%if %{osu114}
install -D -m775 redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}d
%endif
%endif

%if %{fedora}
%if %{fc17} || %{fc18}
install -D -m644 redhat/%{systemctl_configuration} $RPM_BUILD_ROOT%{_unitdir}/wolframed.service
%endif
%endif

%if %{suse}
%if %{osu122} || %{osu131}
install -D -m644 redhat/%{systemctl_configuration} $RPM_BUILD_ROOT%{_unitdir}/wolframed.service
%endif
%endif

install -D -m644 redhat/%{configuration} $RPM_BUILD_ROOT%{_sysconfdir}/wolframe/wolframe.conf

install -d -m775 $RPM_BUILD_ROOT%{_localstatedir}/log/wolframe

%if %{fedora}
%if %{fc17} || %{fc18}
install -D -m644 redhat/%{firewalld_configuration} $RPM_BUILD_ROOT%{_prefix}/lib/firewalld/services/wolframe.xml
%endif
%endif

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

if test ! -d /var/run/wolframe; then
  mkdir /var/run/wolframe
  chown %{WOLFRAME_USR}:%{WOLFRAME_GRP} /var/run/wolframe
  chmod 0755 /var/run/wolframe
fi
 
# Don't enable Wolframe server at install time, just inform root how this is done
%if %{rhel} || %{centos} || %{sles}
echo
echo "Use '/sbin/chkconfig --add wolframed' and '/sbin/chkconfig wolframed on' to enable the"
echo Wolframe server at startup
echo
%endif
%if %{fedora}
%if %{fc17} || %{fc18}
echo
echo "Use 'systemctl enable wolframed.service' to enable the server at startup"
echo
echo "Use 'firewall-cmd --add-service=wolframe' to set the firewall rules"
echo
%endif
%endif
%if %{suse}
%if %{osu122} || %{osu131}
echo
echo "Use 'systemctl enable wolframed.service' to enable the server at startup"
echo
%else
echo
echo "Use '/sbin/chkconfig --add wolframed' and '/sbin/chkconfig wolframed on' to enable the"
echo Wolframe server at startup
echo
%endif
echo
echo "Add the Wolframe ports to 'FW_SERVICES_EXT_TCP' in '/etc/sysconfig/SuSEfirewall2'"
echo and restart the firewall.
echo
%endif

%preun
if [ "$1" = 0 ]; then
%if %{rhel} || %{centos} || %{sles}
    /etc/init.d/wolframed stop > /dev/null 2>&1
    /sbin/chkconfig --del wolframed
%endif
%if %{fedora}
systemctl stop wolframed.service   
systemctl disable wolframed.service
%endif
%if %{suse}
%if %{osu114}
    /etc/init.d/wolframed stop > /dev/null 2>&1
    /sbin/chkconfig --del wolframed
%endif
%if %{osu122} || %{osu131}
systemctl stop wolframed.service
systemctl disable wolframed.service
%endif
%endif
  if test -d /var/run/wolframe; then
    rmdir /var/run/wolframe
  fi
fi

%postun
if [ "$1" = 0 ]; then
    /usr/sbin/userdel %{WOLFRAME_USR}
fi

%files
%defattr( -, root, root )
%if %{rhel} || %{centos} || %{sles}
%attr( 554, root, root) %{_initrddir}/%{name}d
%endif
%if %{suse}
%if %{osu114}
%attr( 554, root, root) %{_initrddir}/%{name}d
%endif
%endif

%if %{fedora}
%if %{fc17} || %{fc18}
%dir %attr(0755, root, root) %{_unitdir}
%{_unitdir}/wolframed.service
%endif
%endif
%if %{suse}
%if %{osu122} || %{osu131}
%{_unitdir}/wolframed.service
%endif
%endif
%{_sbindir}/wolframed
%{_bindir}/wolfilter
%{_bindir}/wolfpasswd
%{_bindir}/wolfwizard
%dir %attr(0755, root, root) %{_sysconfdir}/wolframe
%config %attr(0644, root, root) %{_sysconfdir}/wolframe/wolframe.conf
%if %{fedora}
%if %{fc17} || %{fc18}
%{_prefix}/lib/firewalld/services/wolframe.xml
%endif
%endif
%attr(0775, %{WOLFRAME_USR}, %{WOLFRAME_GRP}) %dir %{_localstatedir}/log/wolframe
%if !%{sles}
%dir %attr(0755, root, root) %{_mandir}/man5
%endif
%{_mandir}/man5/wolframe.conf.5.gz
%if !%{sles}
%dir %attr(0755, root, root) %{_mandir}/man8
%endif
%{_mandir}/man8/wolframed.8.gz
%if !%{sles}
%dir %attr(0755, root, root) %{_mandir}/man1
%endif
%{_mandir}/man1/wolfpasswd.1.gz

%if %{build_boost}
%{_libdir}/wolframe/libboost_program_options.so.%{boost_version}
%{_libdir}/wolframe/libboost_system.so.%{boost_version}
%{_libdir}/wolframe/libboost_filesystem.so.%{boost_version}
%{_libdir}/wolframe/libboost_thread.so.%{boost_version}
%{_libdir}/wolframe/libboost_date_time.so.%{boost_version}
%{_libdir}/wolframe/libboost_locale.so.%{boost_version}
%{_libdir}/wolframe/libboost_regex.so.%{boost_version}
%endif

%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libwolframe.so.0.0.0
%{_libdir}/wolframe/libwolframe.so.0
%{_libdir}/wolframe/libwolframe_serialize.so.0.0.0
%{_libdir}/wolframe/libwolframe_serialize.so.0
%{_libdir}/wolframe/libwolframe_database.so.0.0.0
%{_libdir}/wolframe/libwolframe_database.so.0
%{_libdir}/wolframe/libwolframe_langbind.so.0.0.0
%{_libdir}/wolframe/libwolframe_langbind.so.0
%{_libdir}/wolframe/libwolframe_prnt.so.0.0.0
%{_libdir}/wolframe/libwolframe_prnt.so.0

%if %{with_lua}
%{_libdir}/wolframe/liblua.so.5.2.0
%{_libdir}/wolframe/liblua.so.5
%endif

%dir %{_libdir}/wolframe/modules

%{_libdir}/wolframe/modules/mod_audit_textfile.so
%{_libdir}/wolframe/modules/mod_audit_database.so

%{_libdir}/wolframe/modules/mod_auth_textfile.so
%{_libdir}/wolframe/modules/mod_auth_database.so

%{_libdir}/wolframe/modules/mod_authz_database.so

%{_libdir}/wolframe/modules/mod_filter_char.so
%{_libdir}/wolframe/modules/mod_filter_line.so
%{_libdir}/wolframe/modules/mod_filter_textwolf.so
%{_libdir}/wolframe/modules/mod_filter_token.so
%{_libdir}/wolframe/modules/mod_filter_blob.so

%{_libdir}/wolframe/modules/mod_command_directmap.so
%{_libdir}/wolframe/modules/mod_ddlcompiler_simpleform.so

%if %{with_lua}
%{_libdir}/wolframe/modules/mod_lua_bcdnumber.so
%{_libdir}/wolframe/modules/mod_lua_datetime.so
%{_libdir}/wolframe/modules/mod_command_lua.so
%endif

%{_libdir}/wolframe/modules/mod_normalize_number.so
%{_libdir}/wolframe/modules/mod_normalize_base64.so
%{_libdir}/wolframe/modules/mod_normalize_string.so

%if %{with_icu}
%{_libdir}/wolframe/modules/mod_normalize_locale.so
%endif

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
%{_libdir}/wolframe/libwolframe_serialize.so
%{_libdir}/wolframe/libwolframe_serialize.a
%{_libdir}/wolframe/libwolframe_database.so
%{_libdir}/wolframe/libwolframe_database.a
%{_libdir}/wolframe/libwolframe_langbind.so
%{_libdir}/wolframe/libwolframe_langbind.a
%{_libdir}/wolframe/libwolframe_prnt.so
%{_libdir}/wolframe/libwolframe_prnt.a
%if %{with_lua}
%{_libdir}/wolframe/liblua.so
%{_libdir}/wolframe/liblua.a
%dir %{_includedir}/wolframe/lua
%{_includedir}/wolframe/lua/*.h
%{_includedir}/wolframe/lua/*.hpp
%endif
%if %{build_libhpdf}
%{_libdir}/wolframe/libhpdf.so
%{_libdir}/wolframe/libhpdf.a
%dir %{_includedir}/wolframe/libhpdf
%{_includedir}/wolframe/libhpdf/*.h
%endif
%if %{build_freeimage}
%{_libdir}/wolframe/libfreeimage.so
%{_libdir}/wolframe/libfreeimage.a
%{_libdir}/wolframe/libfreeimageplus.so
%{_libdir}/wolframe/libfreeimageplus.a
%endif
%dir %{_includedir}/wolframe
%{_includedir}/wolframe/*.hpp
%dir %{_includedir}/wolframe/langbind/
%{_includedir}/wolframe/langbind/*.hpp
%dir %{_includedir}/wolframe/protocol/
%{_includedir}/wolframe/protocol/*.hpp
%dir %{_includedir}/wolframe/cmdbind/
%{_includedir}/wolframe/cmdbind/*.hpp
%dir %{_includedir}/wolframe/database/
%{_includedir}/wolframe/database/*.hpp
%dir %{_includedir}/wolframe/prnt/
%{_includedir}/wolframe/prnt/*.hpp
%dir %{_includedir}/wolframe/config/
%{_includedir}/wolframe/config/*.hpp
%dir %{_includedir}/wolframe/ddl/
%{_includedir}/wolframe/ddl/*.hpp
%dir %{_includedir}/wolframe/logger/
%{_includedir}/wolframe/logger/*.hpp
%dir %{_includedir}/wolframe/utils/
%{_includedir}/wolframe/utils/*.hpp
%dir %{_includedir}/wolframe/serialize/
%{_includedir}/wolframe/serialize/*.hpp
%dir %{_includedir}/wolframe/serialize/struct/
%{_includedir}/wolframe/serialize/struct/*.hpp
%dir %{_includedir}/wolframe/serialize/ddl/
%{_includedir}/wolframe/serialize/ddl/*.hpp
%dir %{_includedir}/wolframe/processor/
%{_includedir}/wolframe/processor/*.hpp
%dir %{_includedir}/wolframe/filter/
%{_includedir}/wolframe/filter/*.hpp
%dir %{_includedir}/wolframe/types/
%{_includedir}/wolframe/types/*.hpp
%{_includedir}/wolframe/types/*.h
%dir %{_includedir}/wolframe/AAAA/
%{_includedir}/wolframe/AAAA/*.hpp
%dir %{_includedir}/wolframe/module/
%{_includedir}/wolframe/module/*.hpp
%dir %{_includedir}/wolframe/prgbind/
%{_includedir}/wolframe/prgbind/*.hpp

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

%if %{with_pam}
%files pam
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_auth_pam.so
%endif

%if %{with_sasl}
%files sasl
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_auth_sasl.so
%endif

%if %{with_libxml2}
%files libxml2
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_filter_libxml2.so
%if %{build_libxml2}
%{_libdir}/wolframe/libxml2.so.%{libxml2_version}
%{_libdir}/wolframe/libxml2.so.2
%endif
%endif

%if %{with_libxslt}
%files libxslt
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
# later:
#%{_libdir}/wolframe/modules/mod_filter_libxslt.so
%endif

%if %{with_libhpdf}
%files libhpdf
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_haru_pdf_printer.so
%if %{build_libhpdf}
%{_libdir}/wolframe/libhpdf.so.2.2.1
%{_libdir}/wolframe/libhpdf.so.2
%endif

%if %{with_freeimage}
%files freeimage
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_graphix.so
%if %{build_freeimage}
%{_libdir}/wolframe/libfreeimage.so.3.15.4
%{_libdir}/wolframe/libfreeimage.so.3
%{_libdir}/wolframe/libfreeimageplus.so.3.15.4
%{_libdir}/wolframe/libfreeimageplus.so.3
%endif
%endif

%endif

%files libclient
%defattr( -, root, root )
# funny, why?!
%if !%{sles}
%dir %{_bindir}
%endif
%if !%{sles}
%dir %{_libdir}/wolframe/
%endif
%{_libdir}/wolframe/libwolframe_client.so.0.0.0
%{_libdir}/wolframe/libwolframe_client.so.0

%files libclient-devel
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libwolframe_client.so
%{_libdir}/wolframe/libwolframe_client.a
%dir %{_includedir}/wolframe/libclient
%{_includedir}/wolframe/libclient/*.hpp

%files client
%defattr( -, root, root )
# funny, why?!
%if !%{sles}
%dir %{_bindir}
%endif
%{_bindir}/wolframec

%if %{with_qt4}
%files qtclient
%defattr( -, root, root )
# funny, why?!
%if !%{sles}
%dir %{_bindir}
%endif
%{_bindir}/qtclient
%dir %{_libdir}/wolframe/plugins/
%{_libdir}/wolframe/plugins/libwolframewidgets.so
%endif

%changelog
* Sun Aug 29 2011 Andreas Baumann <abaumann@yahoo.com> 0.0.1-0.2
- more splitting into sub-packages for modules
- builds on OpenSuse Build Service (osc)

* Sun Aug 29 2010 Mihai Barbos <mihai.barbos@gmail.com> 0.0.1-0.1
- preliminary release
