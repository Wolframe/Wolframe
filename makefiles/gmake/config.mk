# Configuration supposed to be configured here by the user
#
# provides:
# - BOOST_DIR
# - OPENSSL_DIR
# - LUA_DIR

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# base dir where boost is installed

BOOST_DIR=/usr
#BOOST_DIR=/home/baumann/boost

# TODO: probe those
BOOST_MT=-mt

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

OPENSSL_DIR=/usr
#OPENSSL_DIR=/home/baumann/openssl

# LUA sources (for LUA handler)
# build it from sources with:
#  cd lua-5.1.4
#  etc\luavc.bat
###############################

#LUA_DIR = ..\..\lua-5.1.4
LUA_DIR = /home/baumann/lua-5.1.4
