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

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

BOOST_VERSION=1_44

# base dir where boost is installed

BOOST_DIR=C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR=C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR=D:\Programme\boost\boost_$(BOOST_VERSION)

# visual studio version used for compiling

BOOST_VC_VER=vc80
#BOOST_VC_VER=vc90

# TODO: probe those
BOOST_MT=-mt

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

OPENSSL_DIR=C:\Programme\OpenSSL-1.0.0a
#OPENSSL_DIR=C:\Program Files\OpenSSL
#OPENSSL_DIR=D:\Programme\OpenSSL
