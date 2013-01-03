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
# - LIBXML2_DIR: location of libxml2 (WITH_LIBXML2=1 only)
# - LIBXSLT_DIR: location of libxslt (WITH_LIBXSLT=1 only)
# - TCL_DIR: location of the Active Tcl installation (for testing only)
# - EXPECT: location of the expect interpreter (for testing only)

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library
BOOST_VERSION = 1_49

# base dir where boost is installed
BOOST_DIR = C:\boost\boost-1.49.0

# depends on the choosen boost layout
BOOST_INCLUDE_DIR = "$(BOOST_DIR)\include"

# visual studio version used for compiling
BOOST_VC_VER = vc100

# TODO: probe those
#BOOST_MT = -mt

# preinstalled binaries
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib"


# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################
OPENSSL_DIR = C:\OpenSSL-Win64

# Qt 4 (http://qt.nokia.com/products/)
# take the VC2008 version SDK Opensource, not the MinGW one
############################################################
QT_DIR = C:\Qt\4.7.4

# Postgresql libpq
# (http://www.postgresql.org/)
##############################
PGSQL_DIR = C:\Program Files\PostgreSQL\9.1

# libxml2
#########
#LIBXML2_DIR = D:\Program Files\libxml2-2.7.8.win32

# libxslt
#########
#LIBXSLT_DIR = D:\Program Files\libxslt-1.1.24.win32

# Tcl 8.5 and Expect
####################
#TCL_DIR = C:\Program Files (x86)\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe

# Windows Installer (WIX)
# used for deployment only!
#########################

# @office
WIX_DIR = C:\Program Files (x86)\Windows Installer XML v3.5

