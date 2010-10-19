# Configuration supposed to be configured here by the user
#
# provides:
# - BOOST_DIR
# - OPENSSL_DIR

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# base dir where boost is installed

BOOST_DIR=C:\Program Files\boost\boost_$(BOOST_VERSION)

# TODO: probe those
BOOST_MT=-mt

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

OPENSSL_DIR=C:\Program Files\OpenSSL

# LUA sources (for LUA handler)
# build it from sources with:
#  cd lua-5.1.4
#  etc\luavc.bat
###############################

LUA_DIR = ..\..\lua-5.1.4
