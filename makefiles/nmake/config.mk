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

PLATFORM_SDK_DIR = C:\Programme\Microsoft Platform SDK
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2
#PLATFORM_SDK_DIR = D:\Programme\Microsoft Platform SDK
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.0
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.1

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

BOOST_VERSION=1_44

# base dir where boost is installed

BOOST_DIR=C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR=C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR=D:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR=C:\Cygwin\home\Andreas\boost_1_45_0
#BOOST_DIR=C:\Cygwin\home\Administrator\boost_1_45_0

# visual studio version used for compiling

BOOST_VC_VER=vc80
#BOOST_VC_VER=vc90
#BOOST_VC_VER=vc100

# TODO: probe those
BOOST_MT=-mt

# preinstalled binaties
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib"

# self-compiled
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\stage\lib"

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

OPENSSL_DIR=C:\Programme\OpenSSL-1.0.0a
#OPENSSL_DIR=C:\Program Files\OpenSSL
#OPENSSL_DIR=D:\Programme\OpenSSL

# Qt 4 (http://qt.nokia.com/products/)
# take the VC2008 version SDK Opensource, not the MinGW one
############################################################

QT_DIR=C:\Qt\4.7.1
#QT_DIR=/home/user/qt
