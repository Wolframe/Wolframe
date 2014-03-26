# Configuration supposed to be configured here by the user
#
# must configure:
# - BOOST_VERSION: version of boost
# - BOOST_DIR: location of boost intallation
# - BOOST_INCLUDE_DIR and BOOST_LDFLAGS
# - BOOST_VC_VER: visual studio boost version tag for libraries
#
# optionally:
# - OPENSSL_DIR: location of the OpenSSL library (WITH_SSL=1 only)
# - PGSQL_DIR: location of Postgres libpq and header files (WITH_PGSQL=1 only)
# - TCL_DIR: location of the Active Tcl installation (for testing only)
# - EXPECT: location of the expect interpreter (for testing only)

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library
BOOST_VERSION = 1_55

# base dir where boost is installed
#BOOST_DIR = C:\develop\boost_1_55_0
BOOST_DIR = C:\develop\boost_1_55_0-win32-debug

# depends on the choosen boost layout
# pre-compiled
#BOOST_INCLUDE_DIR = $(BOOST_DIR)
# self-compiled
BOOST_INCLUDE_DIR = $(BOOST_DIR)\include\boost-$(BOOST_VERSION)

# visual studio version used for compiling
BOOST_VC_VER = vc100

# TODO: probe those
BOOST_MT = -mt

# self-compiled
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib"

# pre-compiled
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib32-msvc-10.0

# ICU (icu4c)
#############

# pre-compiled
#ICU_DIR = C:\develop\icu4c-52_1-Win32-msvc10\icu

# self-compiled
ICU_DIR = C:\develop\icu4c-52_1-win32-debug

# the library version (how the DLLs are tagged)
ICU_LIB_VERSION = 52

ICU_LIB_DIR = $(ICU_DIR)\lib

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

OPENSSL_DIR = C:\develop\OpenSSL-Win32
#OPENSSL_DIR = C:\Programme\OpenSSL-1.0.0a
#OPENSSL_DIR = c:\OpenSSL\openssl-1.0.1-win32-debug
#OPENSSL_DIR = D:\Programme\OpenSSL
#OPENSSL_DIR = D:\OpenSSL\Win32
#OPENSSL_DIR = D:\Program Files\OpenSSL-1.0.0d

# Postgresql libpq
# (http://www.postgresql.org/)
##############################

PGSQL_DIR = C:\Program Files\PostgreSQL\9.3
#PGSQL_DIR = C:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = D:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = C:\cygwin\home\Andreas Baumann\postgresql-9.1.3-win32-debug

# enable if Postgresql has been compiled with internationalization support
# (libintl and libiconv)
PGDLL_WITH_I18N = 1

# enable depending on libintl.dll and libiconv.dll (deployment only)
PGDLL_WITHOUT_MAJOR_VERSION = 1

# Oracle
########

ORACLE_DIR = C:\develop\Oracle\instantclient_12_1

# zlib
######

ZLIB_DIR = C:\develop\zlib-1.2.5.win32

# iconv
#######

ICONV_DIR = C:\develop\iconv-1.9.2.win32

# libxml2
#########

LIBXML2_DIR = C:\develop\libxml2-2.7.8.win32

# libxslt
#########

LIBXSLT_DIR = C:\develop\libxslt-1.1.26.win32

# Python 3
##########

PYTHON_DIR = C:\develop\Python34
PYTHON_VERSION = 34
PYTHON_MAJOR_VERSION = 3

PYTHON_LIB_DIR = $(PYTHON_DIR)\libs
PYTHON_DLL_DIR = $(PYTHON_DIR)\DLLs

# Tcl 8.5 and Expect
####################

TCL_DIR = C:\Program Files\Tcl
#TCL_DIR = D:\Program Files\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe

# Windows Installer (WIX)
# used for deployment only!
#########################

WIX_DIR = C:\Program Files\Windows Installer XML v3.5

#WIX_LIBS = dutil_2008.lib wcautil_2008.lib
WIX_LIBS = dutil_2008_x64.lib wcautil_2008_x64.lib

# architecture of resulting msi
WIX_ARCH = x86
#WIX_ARCH = x64

#PGDLL_LIBRARIES=1

#PGDLL_WITHOUT_MAJOR_VERSION=1

# define for 64 bit, undefine for 32 bit
#WIN64=1

# DocBook and Stylesheets
#########################

XSLT_HTMLHELP_STYLESHEET = C:\cygwin\home\Andreas Baumann\docbook-xsl-1.76.1\htmlhelp\htmlhelp.xsl

# Doxygen binary to generate API documentation (for docu and deployment)
########################################################################

DOXYGEN = C:\Program Files\Doxygen\bin\doxygen.exe

# Microsoft HTML Help Workshop (for documentation building and deployment)
##########################################################################

HHC_LOCATION = C:\Program Files\HTML Help Workshop\hhc.exe

# CCacche
#########

CC=C:\cygwin\bin\ccache.exe cl
CXX=C:\cygwin\bin\ccache.exe cl
