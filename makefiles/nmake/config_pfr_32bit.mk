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

BOOST_VERSION = 1_55_0
# base dir where boost is installed

BOOST_DIR = C:\Boost\boost_$(BOOST_VERSION)\boost_$(BOOST_VERSION)

# depends on the choosen boost layout
BOOST_INCLUDE_DIR = "$(BOOST_DIR)" 

# visual studio version used for compiling

BOOST_VC_VER = vc100

# TODO: probe those
#BOOST_MT = -mt

# self-compiled
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\stage\lib"

# Postgresql libpq
# (http://www.postgresql.org/)
##############################
PGSQL_DIR = C:\Program Files (x86)\PostgreSQL\9.3

# OpenSSL
###########
OPENSSL_DIR=C:\OpenSSL-Win32\include

# win_iconv
###########
WIN_ICONV_DIR = C:\Iconv\iconv-1.9.2

# libxml2
#########
LIBXML2_DIR = C:\LibXML2\libxml2-2.7.8
