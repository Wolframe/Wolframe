# Configuration supposed to be configured here by the user
#
# provides:
# - PLATFORM_SDK_DIR
# - BOOST_DIR
# - OPENSSL_DIR

# please customize

# Windows Platform SDK
######################

# The location of the Windows Platform SDK
# newer versions of Visual Studio integrate the header files of the SDK
# some versions of Visual Studio miss the mc.exe binary

#PLATFORM_SDK_DIR = C:\Programme\Microsoft Platform SDK
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2
#PLATFORM_SDK_DIR = D:\Programme\Microsoft Platform SDK
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v6.0A
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.0
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.1
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v6.0A
PLATFORM_SDK_DIR =  D:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

BOOST_VERSION = 1_46_1

# base dir where boost is installed

#BOOST_DIR = C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = D:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Cygwin\home\Andreas\boost_1_46_0
#BOOST_DIR = C:\Cygwin\home\Administrator\boost_1_45_0
#BOOST_DIR = D:\boost\boost_1_45_0
BOOST_DIR = D:\Program Files\boost\boost_$(BOOST_VERSION)

# visual studio version used for compiling

#BOOST_VC_VER = vc80
BOOST_VC_VER = vc90
#BOOST_VC_VER = vc100

# TODO: probe those
BOOST_MT = -mt

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
# OPENSSL_DIR = C:\Programme\OpenSSL
#OPENSSL_DIR = C:\Program Files\OpenSSL
#OPENSSL_DIR = D:\Programme\OpenSSL
#OPENSSL_DIR = D:\OpenSSL\Win32
OPENSSL_DIR = D:\Program Files\OpenSSL-1.0.0d

# Qt 4 (http://qt.nokia.com/products/)
# take the VC2008 version SDK Opensource, not the MinGW one
############################################################

#QT_DIR = C:\Qt\4.7.1
#QT_DIR = /home/user/qt
QT_DIR = D:\Qt\4.7.2

# Postgresql libpq
# (http://www.postgresql.org/)
##############################

PGSQL_DIR = D:\Program Files\PostgreSQL\9.0

# libxml2
#########

LIBXML2_DIR = D:\Program Files\libxml2-2.7.8.win32
ICONV_DIR = D:\Program Files\iconv-1.9.2.win32
ZLIB_DIR = D:\Program Files\zlib-1.2.5

# Tcl 8.5 and Expect
####################

#TCL_DIR = D:\Tcl8.5
TCL_DIR = D:\Program Files\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe
