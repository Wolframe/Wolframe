# Configuration supposed to be configured here by the user
#
# provides:
# - BOOST_DIR
# - OPENSSL_DIR
# - QT_DIR
# - LUA_DIR

# IMPORTANT:
# please customize for new platforms or leave commented out for
# an autodetection in platform.mk

# XSLT processor
################

#XSLTPROC = xsltproc

# DocBook and Stylesheets
#########################

#XSLT_MAN_STYLESHEET = /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl

# Boost (http://www.boostpro.com/download)
###########################################

# base dir where boost is installed

# system version
BOOST_DIR=/usr/local
BOOST_LIB_DIR=$(BOOST_DIR)/lib
BOOST_INCLUDE_DIRS=$(BOOST_DIR)/include
BOOST_LIBRARY_TAG=

# self-compiled
#BOOST_DIR=/home/abaumann/boost_1_45_0
#BOOST_INCLUDE_DIRS=$(BOOST_DIR)
#BOOST_LIB_DIR=$(BOOST_DIR)/stage/lib
#BOOST_LIBRARY_TAG=-mt

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

# system version
#OPENSSL_DIR=/usr

# self-compiled
#OPENSSL_DIR=/home/user/openssl

# gtest (http://code.google.com/p/googletest)
#############################################

# system version
#GTEST_DIR=/usr
#GTEST_LIB_DIR=$(GTEST_DIR)/lib
#GTEST_INCLUDE_DIRS=$(GTEST_DIR)/include
#GTEST_LIBS = -lgtest

# self-compiled
#GTEST_DIR=/home/abaumann/gtest-1.5.0
#GTEST_INCLUDE_DIRS=$(GTEST_DIR)/include
#GTEST_LIB_DIR=$(GTEST_DIR)/lib/.libs
#GTEST_LIBS = -lgtest

# Qt 4 (http://qt.nokia.com/products/)
######################################

# self-compiled
#QT_DIR=/home/user/qt

# system-wide
#QT_DIR=/usr

# usual layout, may differ
#QT_INCLUDE_DIRS=$(QT_DIR)/include
#QT_LIB_DIRS=$(QT_DIR)/lib

# Lua 5.1
#########

# self-compiled
#LUA_DIR=/home/user/lua-5.1.4

# system-wide
#LUA_DIR=/usr

# usual layout
#LUA_INCLUDE_DIRS=$(LUA_DIR)/include
#LUA_LIB_DIRS=$(LUA_DIR)/lib
#LUA_LIBS=-llua
