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
# - QT_DIR: location of Qt (WITH_QT=1 only)
# - PGSQL_DIR: location of Postgres libpq and header files (WITH_PGSQL=1 only)
# - TCL_DIR: location of the Active Tcl installation (for testing only)
# - EXPECT: location of the expect interpreter (for testing only)

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

BOOST_VERSION = 1_46_1
#BOOST_VERSION = 1_46_0

# base dir where boost is installed

#BOOST_DIR = C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = D:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Cygwin\home\Andreas\boost_1_46_0-win64
#BOOST_DIR = D:\boost\boost_1_45_0
BOOST_DIR = D:\Program Files\boost\boost_$(BOOST_VERSION)

# depends on the choosen boost layout
#BOOST_INCLUDE_DIR = $(BOOST_DIR)\Include\boost-1_46
BOOST_INCLUDE_DIR = $(BOOST_DIR)

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

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

#OPENSSL_DIR = C:\Programme\OpenSSL-1.0.0a
#OPENSSL_DIR = C:\Program Files\OpenSSL
#OPENSSL_DIR = D:\Programme\OpenSSL
#OPENSSL_DIR = D:\OpenSSL\Win32
OPENSSL_DIR = D:\Program Files\OpenSSL-1.0.0d

# Qt 4 (http://qt.nokia.com/products/)
# take the VC2008 version SDK Opensource, not the MinGW one
############################################################

#QT_DIR = C:\Qt\4.7.3
#QT_DIR = /home/user/qt
QT_DIR = D:\Qt\4.7.2

# Postgresql libpq
# (http://www.postgresql.org/)
##############################

#PGSQL_DIR = C:\Program Files\PostgreSQL\9.0
PGSQL_DIR = D:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = C:\cygwin\home\Andreas\postgresql-9.0.4-win64-debug

# win_iconv
###########

WIN_ICONV_DIR = D:\cygwin\home\Andreas Baumann\win-iconv-0.0.3-win32

# libxml2
#########

LIBXML2_DIR = D:\cygwin\home\Andreas Baumann\libxml2-2.7.8-win32

# libxslt
#########

LIBXSLT_DIR = D:\cygwin\home\Andreas Baumann\libxslt-1.1.26-win32

# Tcl 8.5 and Expect
####################

#TCL_DIR = C:\Program Files (x86)\Tcl8.5.9
TCL_DIR = D:\Program Files\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe
