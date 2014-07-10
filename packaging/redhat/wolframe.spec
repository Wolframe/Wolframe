# Wolframe RPM spec file
#
# Copyright (C) 2011-2014 Project Wolframe

# Set distribution based on some OpenSuse and distribution macros
# this is only relevant when building on https://build.opensuse.org
###

%define rhel 0
%define rhel5 0
%define rhel6 0
%define rhel7 0
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
%if 0%{?rhel_version} >= 700 && 0%{?rhel_version} <= 799
%define dist rhel7
%define rhel 1
%define rhel7 1
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

%define centos7 0
%if 0%{?centos_version} >= 700 && 0%{?centos_version} <= 799
%define dist centos7
%define centos 1
%define centos7 1
%endif

%define scilin 0
%define scilin5 0
%if 0%{?scilin_version} >= 500 && 0%{?scilin_version} <= 599
%define dist scilin5
%define scilin 1
%define scilin5 1
%endif

%define scilin6 0
%if 0%{?scilin_version} >= 600 && 0%{?scilin_version} <= 699
%define dist scilin6
%define scilin 1
%define scilin6 1
%endif

%define scilin7 0
%if 0%{?scilin_version} >= 700 && 0%{?scilin_version} <= 799
%define dist scilin7
%define scilin 1
%define scilin7 1
%endif

%define fedora 0
%define fc19 0
%if 0%{?fedora_version} == 19
%define dist fc19
%define fc19 1
%define fedora 1
%endif
%define fc20 0
%if 0%{?fedora_version} == 20
%define dist fc20
%define fc20 1
%define fedora 1
%endif

%define suse 0
%define osu122 0
%define osu123 0
%define osu131 0
%if 0%{?suse_version} == 1220
%define dist osu122
%define osu122 1
%define suse 1
%endif
%if 0%{?suse_version} == 1230
%define dist osu123
%define osu123 1
%define suse 1
%endif
%if 0%{?suse_version} == 1310
%define dist osu131
%define osu131 1
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
# careful with license issues if you enable Oracle!
%define with_oracle	0
%define with_lua	1
%define with_python	1
%define with_pam	1
%define with_sasl	1
%define with_textwolf	1
%define with_libxml2	1
%define with_libxslt	1
%define with_libhpdf	1
%define with_freeimage	1
%define with_cjson	1
%define with_examples	1

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
%if %{rhel} || %{centos} || %{scilin} || %{sles}
%if %{rhel5} || %{centos5} || %{scilin5} || %{rhel6} || %{centos6} || %{scilin6} || %{sles}
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
%define with_icu	1
%endif
%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
%define with_icu	1
%endif
%endif
%endif

%if %{build_boost}
%define with_icu	1
%if %{rhel}
%if %{rhel6} || %{rhel7}
# No icu-devel on RHEL6/7 on OSC due to license issues from Redhat!
%define with_icu	0
%endif
%endif
%endif

# Build local libxml2 for distributions which have a too old broken version
# (broken in respect to some character encodings, not broken as such)

%if %{with_libxml2}
%define build_libxml2 0
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel5} || %{centos5} || %{scilin5}
%define build_libxml2 1
%define libxml2_version 2.9.1
%endif
%endif
%endif

# Build local libxslt for distributions for platforms building libxml2
# due to charset problems (make sure we use the local version in this case!)

%if %{with_libxslt}
%define build_libxslt 0
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel5} || %{centos5} || %{scilin5}
%define build_libxslt 1
%define libxslt_version 1.1.28
%endif
%endif
%endif

# Build local Python 3 for distributions which have no packages yet

%if %{with_python}
%define build_python 0
%if %{rhel} || %{centos} || %{scilin} || %{sles}
%define build_python 1
%define python_version 3.3.2
%endif
%endif

# init script to start the daemon

%if %{rhel} || %{centos} || %{scilin}
%define initscript	wolframed.initd.RHEL
%endif
%if %{sles}
%define initscript	wolframed.initd.SuSE
%endif

%define configuration	wolframe.conf

%define systemctl_configuration wolframed.service

%define firewalld_configuration wolframe-firewalld.xml

%define WOLFRAME_USR	wolframe
%define WOLFRAME_GRP	wolframe

Summary: Small and medium enterprise resource planning (Wolframe)
Name: wolframe
Version: 0.0.2
Release: 1
License: Wolframe License
Group: Application/Business
Source: %{name}_%{version}.tar.gz
%if %{build_boost}
Source1: boost_%{boost_underscore_version}.tar.gz
Patch0: boost_%{boost_underscore_version}-gcc-compile.patch
%endif
%if %{build_libxml2}
Source2: libxml2-%{libxml2_version}.tar.gz
%endif
%if %{build_libxslt}
Source3: libxslt-%{libxslt_version}.tar.gz
%endif
%if %{build_python}
Source4: Python-%{python_version}.tar.bz2
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
%if %{scilin}
BuildRequires: sl-release
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
BuildRequires: systemd
%endif
%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
BuildRequires: systemd
%{?systemd_requires}
%endif
%endif

%if %{build_boost}
%if %{with_icu}
%if %{centos} || %{scilin}
BuildRequires: libicu-devel >= 3.6
%endif
%if %{rhel}
%if !%{rhel6} && !%{rhel7}
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
%if %{rhel} || %{centos} || %{scilin}
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
%if %{osu122} || %{osu123}
Requires: libboost_thread1_49_0 >= 1.49.0
Requires: libboost_date_time1_49_0 >= 1.49.0
Requires: libboost_filesystem1_49_0 >= 1.49.0
Requires: libboost_program_options1_49_0 >= 1.49.0
Requires: libboost_system1_49_0 >= 1.49.0
Requires: libboost_regex1_49_0 >= 1.49.0
%endif
%if %{osu131}
Requires: libboost_thread1_53_0 >= 1.53.0
Requires: libboost_date_time1_53_0 >= 1.53.0
Requires: libboost_filesystem1_53_0 >= 1.53.0
Requires: libboost_program_options1_53_0 >= 1.53.0
Requires: libboost_system1_53_0 >= 1.53.0
Requires: libboost_regex1_53_0 >= 1.53.0
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

%if %{with_python}
%if !%{build_python}
BuildRequires: python3-devel >= 3.0
%endif
%endif

%if %{with_libxslt}
%if !%{build_libxslt}
BuildRequires: libxslt-devel >= 1.0
%endif
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
%if %{scilin}
%if %{scilin5}
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
%if %{scilin}
%if %{scilin5}
%define build_sqlite 1
%endif
%endif
%endif

# if we use the system one, we must pick the right version
%if !%{build_sqlite}
%if %{with_sqlite}
%if %{rhel} || %{scilin} || %{centos} || %{fedora}
BuildRequires: sqlite-devel >= 3.0
BuildRequires: sqlite >= 3.0
%endif
%if %{suse} || %{sles}
BuildRequires: sqlite3-devel >= 3.0
BuildRequires: sqlite3 >= 3.0
%endif
%endif
%endif

# Oracle database module
%if %{with_oracle}
BuildRequires: oracle-instantclient12.1-devel
%endif

# build local version of libharu/libpdf
%define build_libhpdf 1
%if %{with_libhpdf}
%if %{fedora}
%define build_libhpdf 0
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
Requires: sqlite >= 3.0
%endif
%if %{suse} || %{sles}
Requires: sqlite3 >= 3.0
%endif
%endif

%if %{with_oracle}
%package oracle
Summary: Wolframe Oracle database module
Group: Application/Business

%description oracle
The Wolframe database module for Oracle (OCI).

Requires: %{name} >= %{version}-%{release}
Requires: oracle-instantclient12.1-basic
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
%if %{rhel} || %{centos} || %{scilin} || %{fedora}
Requires: cyrus-sasl-lib >= 2.1.19
%endif
%if %{suse} || %{sles}
Requires: cyrus-sasl >= 2.1.22
%endif

%endif

%if %{with_lua}
%package lua
Summary: Wolframe Lua language bindings
Group: Application/Business

%description lua
Allows writing business application code in Lua.

Requires: %{name} >= %{version}-%{release}

%package lua-devel
Summary: Development header files and libraries for the Wolframe Lua language binding
Group: Application/Business

%description lua-devel
Development files for the Wolframe Lua language bindings.

Requires: %{name}-lua >= %{version}-%{release}

%endif

%if %{with_python}
%package python
Summary: Wolframe Python language bindings
Group: Application/Business

%description python
Allows writing business application code in Python 3.

Requires: %{name} >= %{version}-%{release}
%if !%{build_python}
Requires: python3 >= 0.0
%endif
%endif

%if %{with_textwolf}
%package textwolf
Summary: Wolframe XML filtering module using textwolf
Group: Application/Business

%description textwolf
The Wolframe XML parsing module using textwolf.

Requires: %{name} >= %{version}-%{release}
%endif

%if %{with_libxml2}
%package libxml2
Summary: Wolframe XML filtering module using libxml2
Group: Application/Business

%description libxml2
The Wolframe XML parsing module using libxml2.
Contains also an XSLT filter using libxslt.

Requires: %{name} >= %{version}-%{release}
%if !%{build_libxml2}
Requires: libxml2 >= 2.6
%endif
%if %{with_libxslt}
%if !%{build_libxslt}
Requires: libxslt >= 1.0
%endif
%endif
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

%if %{with_cjson}
%package cjson
Summary: Wolframe JSON filter module with cjson
Group: Application/Business

%description cjson
Wolframe filter JSON filtering module implemented with
the cjson library.

Requires: %{name} >= %{version}-%{release}

%package cjson-devel
Summary: Development header files and libraries of the CJSON JSON library
Group: Application/Business

%description cjson-devel
Development files of the CJSON JSON library.

Requires: %{name}-cjson >= %{version}-%{release}

%endif

%package libclient
Summary: Wolframe C++ client library
Group: Application/Business

%description libclient
Wolframe client library for C++ (implemented using boost libraries).

%if !%{build_boost}
BuildRequires: boost-devel
%if %{rhel} || %{centos} || %{scilin} || %{fedora}
Requires: boost >= 1.48
Requires: boost-thread >= 1.48
Requires: boost-date-time >= 1.48
Requires: boost-filesystem >= 1.48
Requires: boost-program-options >= 1.48
Requires: boost-system >= 1.48
%endif
%if %{suse}
%if %{osu122} || %{osu123}
Requires: libboost_thread1_49_0 >= 1.49.0
Requires: libboost_date_time1_49_0 >= 1.49.0
Requires: libboost_filesystem1_49_0 >= 1.49.0
Requires: libboost_program_options1_49_0 >= 1.49.0
Requires: libboost_system1_49_0 >= 1.49.0
%if %{osu131}
Requires: libboost_thread1_53_0 >= 1.53.0
Requires: libboost_date_time1_53_0 >= 1.53.0
Requires: libboost_filesystem1_53_0 >= 1.53.0
Requires: libboost_program_options1_53_0 >= 1.53.0
Requires: libboost_system1_53_0 >= 1.53.0
Requires: libboost_regex1_53_0 >= 1.53.0
%endif
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

%prep

%if %{build_boost} && %{build_python} && %{build_libxml2} && %{build_libxslt}
%setup -T -D -b 0 -b 1 -b 2 -b 3 -b 4
cd ../boost_%{boost_underscore_version}
%patch -P 0 -p1
cd ../%{name}-%{version}
%else
%if %{build_boost} && %{build_python}
%setup -T -D -b 0 -b 1 -b 4
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
%if %{build_python}
%setup -T -D -b 0 -b 4
cd ../%{name}-%{version}
%else
%setup
%endif
%endif
%endif
%endif

%build

%if %{build_boost}
cd %{_builddir}/boost_%{boost_underscore_version}
./bootstrap.sh --prefix=/tmp/boost-%{boost_version} \
	--with-libraries=thread,filesystem,system,program_options,date_time,regex,locale
./bjam %{?_smp_mflags} -d1 install
%endif

%if %{build_python}
cd %{_builddir}/Python-%{python_version}
./configure --prefix=/tmp/Python-%{python_version} \
	--enable-shared 
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make %{?_smp_mflags}
make install
%endif

%if %{build_libxml2}
cd %{_builddir}/libxml2-%{libxml2_version}
./configure --prefix=/tmp/libxml2-%{libxml2_version} --without-python
make %{?_smp_mflags}
make install
%endif

%if %{build_libxslt}
cd %{_builddir}/libxslt-%{libxslt_version}
./configure --prefix=/tmp/libxslt-%{libxslt_version} --without-python \
	--with-libxml-prefix=/tmp/libxml2-%{libxml2_version}
make %{?_smp_mflags}
make install
%endif

cd %{_builddir}/%{name}-%{version}
%if %{build_python}
LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib \
MAKE="LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ make" \
%endif
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make help \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=%{with_sqlite} \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
%endif
	sysconfdir=/etc libdir=%{_libdir}

%if %{build_python}
LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib \
MAKE="LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ make" \
%endif
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make config \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
%endif
	sysconfdir=/etc libdir=%{_libdir}

%if %{build_python}
LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib \
MAKE="LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ make" \
%endif
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make depend \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
%endif
	sysconfdir=/etc libdir=%{_libdir}

%if %{build_python}
LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib \
MAKE="LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ make" \
%endif
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make all \
	%{?_smp_mflags} \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
%endif
	sysconfdir=/etc libdir=%{_libdir}

cd docs; make doc-doxygen

%check

echo ======================= TESTING ==============================
%if %{build_python}
LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib \
MAKE="LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ make" \
%endif
LDFLAGS="-Wl,-rpath=%{_libdir}/wolframe,--enable-new-dtags" make test \
	%{?_smp_mflags} \
	RELEASE=1 DEFAULT_MODULE_LOAD_DIR=%{_libdir}/wolframe/modules \
%if %{build_boost}
	BOOST_DIR=/tmp/boost-%{boost_version} \
	%{boost_library_tag} \
%endif
%if %{build_libxml2}
	LIBXML2_DIR=/tmp/libxml2-%{libxml2_version} \
%endif
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
%endif
	sysconfdir=/etc libdir=%{_libdir}
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
%if %{build_libxslt}
	LIBXSLT_DIR=/tmp/libxslt-%{libxslt_version} \
%endif
	WITH_PYTHON=%{with_python} \
%if %{build_python}
	PYTHON3_CONFIG=/tmp/Python-%{python_version}/bin/python3-config \
	PYTHON_DIR=/tmp/Python-%{python_version} \
	PYTHON_LDFLAGS="`LD_LIBRARY_PATH=/tmp/Python-%{python_version}/lib/ \
	/tmp/Python-%{python_version}/bin/python3-config --ldflags` -L/tmp/Python-%{python_version}/lib" \
%endif
%if %{build_sqlite}
	WITH_LOCAL_SQLITE3=%{build_sqlite} \
%else
	WITH_SYSTEM_SQLITE3=1 \
%endif
	WITH_SSL=%{with_ssl} \
	WITH_LUA=%{with_lua} WITH_PAM=%{with_pam} \
	WITH_SASL=%{with_sasl} WITH_PGSQL=%{with_pgsql} \
	WITH_ORACLE=%{with_oracle} \
%if %{with_textwolf}
	WITH_TEXTWOLF=%{with_textwolf} \
%endif
	WITH_LIBXML2=%{with_libxml2} \
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
%if %{with_cjson}
	WITH_CJSON=1 \
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

# dito for local version of libxslt
%if %{build_libxslt}
cp /tmp/libxslt-%{libxslt_version}/lib/libxslt.so.%{libxslt_version} $RPM_BUILD_ROOT%{_libdir}/wolframe/
ln -s libxslt.so.%{libxslt_version} $RPM_BUILD_ROOT%{_libdir}/wolframe/libxslt.so.1
%endif

#TODO: What exactly do we need? Do we have to probe it?
# copy local python library to local library directory for platforms which need it
%if %{build_python}
cp -P /tmp/Python-%{python_version}/lib/libpython* $RPM_BUILD_ROOT%{_libdir}/wolframe
%endif

%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
install -D -m644 packaging/redhat/%{systemctl_configuration} $RPM_BUILD_ROOT%{_unitdir}/wolframed.service
%else
install -D -m775 packaging/redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}d
%endif
%endif

%if %{fedora}
install -D -m644 packaging/redhat/%{systemctl_configuration} $RPM_BUILD_ROOT%{_unitdir}/wolframed.service
%endif

%if %{sles}
install -D -m775 packaging/redhat/%{initscript} $RPM_BUILD_ROOT%{_initrddir}/%{name}d
%endif

%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
install -D -m644 packaging/redhat/%{systemctl_configuration} $RPM_BUILD_ROOT%{_unitdir}/wolframed.service
%endif
%endif

install -D -m644 packaging/redhat/%{configuration} $RPM_BUILD_ROOT%{_sysconfdir}/wolframe/wolframe.conf

install -d -m775 $RPM_BUILD_ROOT%{_localstatedir}/log/wolframe
install -d -m775 $RPM_BUILD_ROOT%{_localstatedir}/run/wolframe

%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
install -D -m644 packaging/redhat/%{firewalld_configuration} $RPM_BUILD_ROOT%{_prefix}/lib/firewalld/services/wolframe.xml
%endif
%endif

%if %{fedora}
install -D -m644 packaging/redhat/%{firewalld_configuration} $RPM_BUILD_ROOT%{_prefix}/lib/firewalld/services/wolframe.xml
%endif

%clean
rm -rf $RPM_BUILD_ROOT


%pre
getent group %{WOLFRAME_GRP} >/dev/null || /usr/sbin/groupadd %{WOLFRAME_GRP}
%if %{rhel} || %{centos} || %{scilin} || %{fedora}
getent passwd %{WOLFRAME_USR} >/dev/null || /usr/sbin/useradd -g %{WOLFRAME_GRP} %{WOLFRAME_USR} -c "Wolframe user" -d /dev/null
%endif
%if %{suse} || %{sles}
getent passwd %{WOLFRAME_USR} >/dev/null || /usr/sbin/useradd -g %{WOLFRAME_GRP} %{WOLFRAME_USR} -c "Wolframe user"
%endif
 
# Don't enable Wolframe server at install time, just inform root how this is done
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
echo
echo "Use 'systemctl enable wolframed.service' to enable the server at startup"
echo
echo "Use 'firewall-cmd --add-service=wolframe' to set the firewall rules"
echo
%else
echo
echo "Use '/sbin/chkconfig --add wolframed' and '/sbin/chkconfig wolframed on' to enable the"
echo Wolframe server at startup
echo
%endif
%endif
%if %{sles}
echo
echo "Use '/sbin/chkconfig --add wolframed' and '/sbin/chkconfig wolframed on' to enable the"
echo Wolframe server at startup
echo
%endif
%if %{fedora}
echo
echo "Use 'systemctl enable wolframed.service' to enable the server at startup"
echo
echo "Use 'firewall-cmd --add-service=wolframe' to set the firewall rules"
echo
%endif
%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
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
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
    systemctl stop wolframed.service   
    systemctl disable wolframed.service
%else
    /etc/init.d/wolframed stop > /dev/null 2>&1
    /sbin/chkconfig --del wolframed
%endif
%endif
%if %{sles}
    /etc/init.d/wolframed stop > /dev/null 2>&1
    /sbin/chkconfig --del wolframed
%endif
%if %{fedora}
    systemctl stop wolframed.service   
    systemctl disable wolframed.service
%endif
%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
    systemctl stop wolframed.service
    systemctl disable wolframed.service
%endif
%endif
    if test -d /var/log/wolframe; then
       rm -rf /var/log/wolframe/*
    fi
fi

%postun
if [ "$1" = 0 ]; then
    /usr/sbin/userdel %{WOLFRAME_USR}
fi

%files
%defattr( -, root, root )
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
%{_unitdir}/wolframed.service
%else
%attr( 554, root, root) %{_initrddir}/%{name}d
%endif
%endif
%if %{fedora}
%dir %attr(0755, root, root) %{_unitdir}
%{_unitdir}/wolframed.service
%endif
%if %{sles}
%attr( 554, root, root) %{_initrddir}/%{name}d
%endif
%if %{suse}
%if %{osu122} || %{osu123} || %{osu131}
%{_unitdir}/wolframed.service
%endif
%endif
%{_sbindir}/wolframed
%{_bindir}/wolfilter
%{_bindir}/wolfpasswd
%{_bindir}/wolfwizard
%dir %attr(0755, root, root) %{_sysconfdir}/wolframe
%config %attr(0644, root, root) %{_sysconfdir}/wolframe/wolframe.conf
%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
%{_prefix}/lib/firewalld/services/wolframe.xml
%endif
%endif
%if %{fedora}
%{_prefix}/lib/firewalld/services/wolframe.xml
%endif
%attr(0775, %{WOLFRAME_USR}, %{WOLFRAME_GRP}) %dir %{_localstatedir}/log/wolframe
%attr(0775, %{WOLFRAME_USR}, %{WOLFRAME_GRP}) %dir %{_localstatedir}/run/wolframe
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
%{_libdir}/wolframe/libwfsingleton.so.0.0.0
%{_libdir}/wolframe/libwfsingleton.so.0
%{_libdir}/wolframe/libwolframe.so.0.0.0
%{_libdir}/wolframe/libwolframe.so.0
%{_libdir}/wolframe/libwolframe_serialize.so.0.0.0
%{_libdir}/wolframe/libwolframe_serialize.so.0
%{_libdir}/wolframe/libwolframe_database.so.0.0.0
%{_libdir}/wolframe/libwolframe_database.so.0
%{_libdir}/wolframe/libwolframe_langbind.so.0.0.0
%{_libdir}/wolframe/libwolframe_langbind.so.0

%dir %{_libdir}/wolframe/modules

%{_libdir}/wolframe/modules/mod_audit_textfile.so
%{_libdir}/wolframe/modules/mod_audit_database.so

%{_libdir}/wolframe/modules/mod_auth_textfile.so
%{_libdir}/wolframe/modules/mod_auth_database.so

%{_libdir}/wolframe/modules/mod_authz_database.so

%{_libdir}/wolframe/modules/mod_filter_blob.so

%{_libdir}/wolframe/modules/mod_command_directmap.so
%{_libdir}/wolframe/modules/mod_ddlcompiler_simpleform.so

%{_libdir}/wolframe/modules/mod_job_schedule_db.so

%{_libdir}/wolframe/modules/mod_normalize_number.so
%{_libdir}/wolframe/modules/mod_normalize_base64.so

%{_libdir}/wolframe/modules/mod_datatype_datetime.so
%{_libdir}/wolframe/modules/mod_datatype_bcdnumber.so

%{_libdir}/wolframe/modules/mod_doctype_xml.so
%{_libdir}/wolframe/modules/mod_doctype_json.so

%if %{with_icu}
%{_libdir}/wolframe/modules/mod_normalize_locale.so
%endif

%{_libdir}/wolframe/modules/mod_command_aamap.so

%{_libdir}/wolframe/modules/mod_command_tdl.so

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
%{_libdir}/wolframe/libwfsingleton.so
%{_libdir}/wolframe/libwfsingleton.a
%{_libdir}/wolframe/libwolframe_serialize.so
%{_libdir}/wolframe/libwolframe_serialize.a
%{_libdir}/wolframe/libwolframe_database.so
%{_libdir}/wolframe/libwolframe_database.a
%{_libdir}/wolframe/libwolframe_langbind.so
%{_libdir}/wolframe/libwolframe_langbind.a
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
%dir %{_includedir}/wolframe/config/
%{_includedir}/wolframe/config/*.hpp
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
%dir %{_includedir}/wolframe/crypto/
%{_includedir}/wolframe/crypto/*.hpp
%{_includedir}/wolframe/crypto/*.h
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
%if !%{sles}
%dir %attr(0755, root, root) %{_mandir}/man5
%endif
%{_mandir}/man5/wolframe-mod_db_sqlite3.5.gz
%endif

%if %{with_oracle}
%files oracle
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_db_oracle.so
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

%if %{with_textwolf}
%files textwolf
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_filter_textwolf.so
%{_libdir}/wolframe/modules/mod_filter_char.so
%{_libdir}/wolframe/modules/mod_filter_line.so
%{_libdir}/wolframe/modules/mod_filter_token.so
%{_libdir}/wolframe/modules/mod_normalize_string.so
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
%if %{build_libxslt}
%{_libdir}/wolframe/libxslt.so.%{libxslt_version}
%{_libdir}/wolframe/libxslt.so.1
%endif
%endif

%if %{with_lua}

%files lua
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_command_lua.so
%{_libdir}/wolframe/liblua.so.5.2.0
%{_libdir}/wolframe/liblua.so.5

%files lua-devel
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%{_libdir}/wolframe/liblua.so
%{_libdir}/wolframe/liblua.a
%dir %{_includedir}/wolframe/lua
%{_includedir}/wolframe/lua/*.h
%{_includedir}/wolframe/lua/*.hpp

%endif

%if %{with_python}
%files python
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_command_python.so
%if %{build_python}
%{_libdir}/wolframe/libpython*
%endif
%endif

%if %{with_libhpdf}
%files libhpdf
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_print_harupdf.so
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

%if %{with_cjson}
%files cjson
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%dir %{_libdir}/wolframe/modules
%{_libdir}/wolframe/modules/mod_filter_cjson.so

%files cjson-devel
%defattr( -, root, root )
%dir %{_libdir}/wolframe
%{_libdir}/wolframe/libcjson.a
%dir %{_includedir}/wolframe/cjson
%{_includedir}/wolframe/cjson/cJSON.h
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

%changelog
* Fri Jun 20 2014 Andreas Baumann <abaumann@yahoo.com> 0.0.2-1
- intermediary release

* Mon Aug 29 2011 Andreas Baumann <abaumann@yahoo.com> 0.0.1-0.2
- more splitting into sub-packages for modules
- builds on OpenSuse Build Service (osc)

* Sun Aug 29 2010 Mihai Barbos <mihai.barbos@gmail.com> 0.0.1-0.1
- preliminary release
