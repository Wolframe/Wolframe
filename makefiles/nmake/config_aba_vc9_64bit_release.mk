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

#BOOST_VERSION = 1_46_1
BOOST_VERSION = 1_49_0

# base dir where boost is installed

#BOOST_DIR = C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = D:\Programme\boost\boost_$(BOOST_VERSION)
BOOST_DIR = C:\Cygwin\home\Andreas\boost_1_49_0-win64
#BOOST_DIR = D:\boost\boost_1_45_0
#BOOST_DIR = D:\Program Files\boost\boost_$(BOOST_VERSION)

# depends on the choosen boost layout
BOOST_INCLUDE_DIR = $(BOOST_DIR)\Include\boost-1_49
#BOOST_INCLUDE_DIR = $(BOOST_DIR)

# visual studio version used for compiling

#BOOST_VC_VER = vc80
BOOST_VC_VER = vc90
#BOOST_VC_VER = vc100

# TODO: probe those
#BOOST_MT = -mt

# preinstalled binaries
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib"

# self-compiled
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\stage\lib"
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib\Win32"

# precompiled libraries from http://boost.teeks99.com
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib32
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib64

# ICU (icu4c)
#############

ICU_DIR = C:\Cygwin\home\Andreas Baumann\icu-49.1.2-win64-release
ICU_DIR = C:\Cygwin\home\Andreas\icu-4.4.2-win64-release

# the library version (how the DLLs are tagged)
#ICU_LIB_VERSION = 49
ICU_LIB_VERSION =

# TODO: probe!
#ICU_LIB_DIR = $(ICU_DIR)\lib
ICU_LIB_DIR = $(ICU_DIR)\lib64

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

#OPENSSL_DIR = C:\Programme\OpenSSL-1.0.0a
OPENSSL_DIR = C:\Program Files\OpenSSL
#OPENSSL_DIR = D:\Programme\OpenSSL
#OPENSSL_DIR = D:\OpenSSL\Win32
#OPENSSL_DIR = D:\Program Files\OpenSSL-1.0.0d

# Postgresql libpq
# (http://www.postgresql.org/)
##############################

PGSQL_DIR = C:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = D:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = C:\cygwin\home\Andreas\postgresql-9.0.4-win64-debug

# enable depending on libintl.dll and libiconv.dll (deployment only)
PGDLL_WITHOUT_MAJOR_VERSION = 1

# win_iconv
###########

WIN_ICONV_DIR = C:\cygwin\home\Andreas\win-iconv-0.0.3-win64

# libxml2
#########

LIBXML2_DIR = C:\cygwin\home\Andreas\libxml2-2.7.8-win64

# libxslt
#########

LIBXSLT_DIR = C:\cygwin\home\Andreas\libxslt-1.1.26-win64

# Tcl 8.5 and Expect
####################

TCL_DIR = C:\Program Files (x86)\Tcl8.5.9
#TCL_DIR = D:\Program Files\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe

# Windows Installer (WIX)
# used for deployment only!
#########################

WIX_DIR = C:\Program Files (x86)\Windows Installer XML v3.5

#WIX_LIBS = dutil_2008.lib wcautil_2008.lib
WIX_LIBS = dutil_2008_x64.lib wcautil_2008_x64.lib

# architecture of resulting msi
#WIX_ARCH = x86
WIX_ARCH = x64

#PGDLL_LIBRARIES=1

#PGDLL_WITHOUT_MAJOR_VERSION=1

# define for 64 bit, undefine for 32 bit
WIN64=1

# DocBook and Stylesheets
#########################

XSLT_HTMLHELP_STYLESHEET = C:\cygwin\home\Andreas\docbook-xsl-1.76.1\htmlhelp\htmlhelp.xsl

# Doxygen binary to generate API documentation (for docu and deployment)
########################################################################

DOXYGEN = C:\Program Files\Doxygen\doxygen.exe

# Microsoft HTML Help Workshop (for documentation building and deployment)
##########################################################################

HHC_LOCATION = C:\Program Files (x86)\HTML Help Workshop\hhc.exe

# CCacche
#########

CC="C:\cygwin\home\Andreas\ccache.exe" "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\cl.exe"
CXX="C:\cygwin\home\Andreas\ccache.exe" "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\cl.exe"
