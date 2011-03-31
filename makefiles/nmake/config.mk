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
PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.0
#PLATFORM_SDK_DIR = C:\Program Files\Microsoft SDKs\Windows\v7.1

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

BOOST_VERSION = 1_44

# base dir where boost is installed

#BOOST_DIR = C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = D:\Programme\boost\boost_$(BOOST_VERSION)
BOOST_DIR = C:\Cygwin\home\Andreas\boost_1_46_0
#BOOST_DIR = C:\Cygwin\home\Administrator\boost_1_45_0
#BOOST_DIR = D:\boost\boost_1_45_0

# visual studio version used for compiling

#BOOST_VC_VER = vc80
BOOST_VC_VER = vc90
#BOOST_VC_VER = vc100

# TODO: probe those
BOOST_MT = -mt

# preinstalled binaries
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib"

# self-compiled
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\stage\lib"
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib\Win32"

# precompiled libraries from http://boost.teeks99.com
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib32
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib64

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

#OPENSSL_DIR = C:\Programme\OpenSSL-1.0.0a
OPENSSL_DIR = C:\Programme\OpenSSL
#OPENSSL_DIR = C:\Program Files\OpenSSL
#OPENSSL_DIR = D:\Programme\OpenSSL
#OPENSSL_DIR = D:\OpenSSL\Win32

# gtest (http://code.google.com/p/googletest)
#############################################

#GTEST_DIR = C:\Cygwin\home\Administrator\gtest-1.5.0
GTEST_DIR = C:\Cygwin\home\Andreas\gtest-1.5.0
#GTEST_DIR = D:\Programme\Cygwin\home\Andreas Baumnn\gtest-1.5.0

GTEST_INCLUDE_DIR = $(GTEST_DIR)\include
GTEST_LIB_DIR = $(GTEST_DIR)\src

# Qt 4 (http://qt.nokia.com/products/)
# take the VC2008 version SDK Opensource, not the MinGW one
############################################################

QT_DIR = C:\Qt\4.7.1
#QT_DIR = /home/user/qt

# Lua 5.1
#########

LUA_VERSION = 5.1

# self-compiled
LUA_DIR = C:\Cygwin\home\Andreas\lua-5.1.4
#LUA_DIR = C:\Cygwin\home\Administrator\lua-5.1.4
#LUA_DIR = D:\Programme\cygwin\home\Andreas Baumann\lua-5.1.4
LUA_INCLUDE_DIRS = "$(LUA_DIR)\src"
LUA_LDFLAGS = /LIBPATH:"$(LUA_DIR)\src"
LUA_LIBS = lua51.lib

