# Configuration supposed to be configured here by the user
#
# provides:
# - BOOST_DIR
# - OPENSSL_DIR
# - QT_DIR

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# base dir where boost is installed

# system version
#BOOST_DIR=/usr
#BOOST_LIB_DIRS=$(BOOST_DIR)/lib
#BOOST_INCLUDE_DIRS=$(BOOST_DIR)/include

# self-compiled
BOOST_DIR=/home/abaumann/boost_1_45_0
BOOST_INCLUDE_DIRS=$(BOOST_DIR)
BOOST_LIB_DIR=$(BOOST_DIR)/stage/lib

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

# system version
OPENSSL_DIR=/usr

# self-compiled
#OPENSSL_DIR=/home/user/openssl

# gtest (http://code.google.com/p/googletest)
#############################################

# system version
#GTEST_DIR=/usr
#GTEST_LIB_DIR=$(GTEST_DIR)/lib
#GTEST_INCLUDE_DIRS=$(GTEST_DIR)/include

# self-compiled
GTEST_DIR=/home/abaumann/gtest-1.5.0
GTEST_INCLUDE_DIRS=$(GTEST_DIR)/include
GTEST_LIB_DIR=$(GTEST_DIR)/lib/.libs

# Qt 4 (http://qt.nokia.com/products/)
######################################

QT_DIR=/usr
#QT_DIR=/home/user/qt
