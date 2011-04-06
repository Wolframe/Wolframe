# sets  e. g. to LINUX, OS_MAJOR_VERSION to 2 and OS_MINOR_VERSION to 6
# by calling the 'guess_env' shell script, where the actual probing happens
# Also sets PLATFORM_COMPILE_FLAGS to be included when compiling C/C++ code
#
# requires:
# - TOPDIR
#
# provides:
# - PLATFORM
# - OS_MAJOR_VERSION and OS_MINOR_VERSION
# - GCC_MAJOR_VERSION and GCC_MINOR_VERSION
# - PLATFORM_COMPILE_FLAGS
# - EXE
# - SO
# - INSTALL
# - BOOST_LIBRARY_TAG
#
# author: Andreas Baumann, abaumann at yahoo dot com

-include $(HOME)/config.mk
-include $(TOPDIR)/makefiles/gmake/platform.mk.vars

PLATFORM ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --platform $(CC) "$(CURDIR)" $(TOPDIR))
OS_MAJOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-major-version $(CC) "$(CURDIR)" $(TOPDIR))
OS_MINOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-minor-version $(CC) "$(CURDIR)" $(TOPDIR))
COMPILER ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --compiler $(CC) "$(CURDIR)" $(TOPDIR))
ifeq "$(PLATFORM)" "LINUX" 
LINUX_DIST ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-dist $(CC) "$(CURDIR)" $(TOPDIR))
LINUX_REV ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-rev $(CC) "$(CURDIR)" $(TOPDIR))
endif

# platform specific flags
#########################

PLATFORM_COMPILE_FLAGS = \
			-D$(PLATFORM) \
			-DOS_MAJOR_VERSION=$(OS_MAJOR_VERSION) \
			-DOS_MINOR_VERSION=$(OS_MINOR_VERSION)

ifeq "$(PLATFORM)" "LINUX"
PLATFORM_COMPILE_FLAGS += \
			-DLINUX_DIST=$(LINUX_DIST) -DLINUX_REV=$(LINUX_REV)
endif

# extensions for binaries
ifeq "$(PLATFORM)" "CYGWIN"
EXE = .exe
else
EXE =
endif

# extensions for shared libraries
# (TOOD: HP/Unix has .shlib, Mac/X has .lib, but we can't test it currently)
SO = .so

# name if the installation program
# (TODO: use the MIT or openradio install-sh script instead?)
INSTALL = $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --install)
ifeq "$(PLATFORM)" "SUNOS"
INSTALL = /usr/ucb/install
else
INSTALL = install
endif

# dynamic linker
################

# the linker library for dynamically loadable modules
# (TODO: check for all platforms, as soon we add loadable modules here from
# old TextWolf)
ifeq "$(PLATFORM)" "LINUX"
LIBS_DL = -ldl
else
LIBS_DL =
endif

# i18n, gettext/libintl
#######################

# enable or disable internationalization code
ENABLE_NLS = 1

ifeq "$(ENABLE_NLS)" "1"

# we relly only on the GNU version, other versions (e.g. Solaris) are not
# so nice..

MSGFMT=msgfmt
MSGMERGE=msgmerge
XGETTEXT=xgettext

# the GNU internationalization support is sometimes in separate libarries
ifeq "$(PLATFORM)" "LINUX"
INCLUDE_FLAGS_LT =
LDFLAGS_LT =
LIBS_LT =
endif

ifeq "$(PLATFORM)" "SUNOS"
INCLUDE_FLAGS_LT = -I/usr/local/include
LDFLAGS_LT = -L/usr/local/lib
LIBS_LT = -lintl
endif

ifeq "$(PLATFORM)" "FREEBSD"
INCLUDE_FLAGS_LT = -I/usr/local/include
LDFLAGS_LT = -L/usr/local/lib
LIBS_LT = -lintl
endif

ifeq "$(PLATFORM)" "OPENBSD"
INCLUDE_FLAGS_LT = -I/usr/local/include
LDFLAGS_LT = -L/usr/local/lib
LIBS_LT = -lintl -liconv
endif

ifeq "$(PLATFORM)" "CYGWIN"
INCLUDE_FLAGS_LT =
LDFLAGS_LT =
LIBS_LT = -lintl
endif

ifeq "$(PLATFORM)" "NETBSD"
INCLUDE_FLAGS_LT =
LDFLAGS_LT =
LIBS_LT = -lintl
endif

PLATFORM_COMPILE_FLAGS +=  $(INCLUDE_FLAGS_LT)

endif

PLATFORM_COMPILE_FLAGS += \
	-DENABLE_NLS=$(ENABLE_NLS) -DLOCALEDIR=\"$(localedir)\"

# TCP/IP, DNS
#############

ifeq "$(PLATFORM)" "LINUX"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET =
endif

ifeq "$(PLATFORM)" "SUNOS"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET = -lsocket -lnsl
endif

ifeq "$(PLATFORM)" "FREEBSD"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET =
endif

ifeq "$(PLATFORM)" "OPENBSD"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET =
endif

ifeq "$(PLATFORM)" "CYGWIN"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET =
endif

ifeq "$(PLATFORM)" "NETBSD"
INCLUDE_FLAGS_NET =
LDFLAGS_NET =
LIBS_NET =
endif

PLATFORM_COMPILE_FLAGS +=  $(INCLUDE_FLAGS_NET)

# XSLT processor
################

XSLTPROC ?= xsltproc

# DocBook and Stylesheets
#########################

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
XSLT_VERSION ?= $(shell pacman -Q | grep docbook-xsl | cut -f 2 -d ' ' | cut -f 1 -d -)
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
ifeq "$(LINUX_REV)" "5"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
endif

# Fedora 14
ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14
ifeq "$(LINUX_REV)" "14"
XSLT_VERSION ?= $(shell rpm -q --queryformat '%{VERSION}' docbook-style-xsl)
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

endif

endif

# Boost
#######

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif

ifeq "$(LINUX_DIST)" "slackware"
BOOST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_LIBRARY_TAG ?= NOT SUPPLIED ON THIS PLATFORM
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
BOOST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_LIBRARY_TAG ?= NOT SUPPLIED ON THIS PLATFORM
endif
endif
ifeq "$(LINUX_REV)" "5"
BOOST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM    
BOOST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_LIBRARY_TAG ?= NOT SUPPLIED ON THIS PLATFORM
endif
endif

ifeq "$(LINUX_DIST)" "suse"
ifeq "$(LINUX_REV)" "11"
BOOST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
BOOST_LIBRARY_TAG ?= NOT SUPPLIED ON THIS PLATFORM
endif
endif

# Fedora 14
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "14"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?= -mt
endif
endif

endif

# OpenSSL
#########

ifdef WITH_SSL

ifeq "$(PLATFORM)" "LINUX"

OPENSSL_LIBS ?= -lssl -lcrypto

endif

endif

# Google Unit testing gtest
###########################

ifdef WITH_GTEST

ifeq "$(PLATFORM)" "LINUX" 

ifeq "$(LINUX_DIST)" "arch"
GTEST_DIR ?= /usr
GTEST_LIB_DIR ?= $(GTEST_DIR)/lib  
GTEST_INCLUDE_DIR ?= $(GTEST_DIR)/include 
GTEST_LIBS ?= -lgtest
endif

ifeq "$(LINUX_DIST)" "slackware"
GTEST_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_LIBS ?= NOT SUPPLIED ON THIS PLATFORM
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
GTEST_DIR ?= /usr
GTEST_LIB_DIR ?= $(GTEST_DIR)/lib  
GTEST_INCLUDE_DIR ?= $(GTEST_DIR)/include 
GTEST_LIBS ?= -lgtest
endif
ifeq "$(LINUX_REV)" "5"
GTEST_DIR ?= /usr
GTEST_LIB_DIR ?= $(GTEST_DIR)/lib  
GTEST_INCLUDE_DIR ?= $(GTEST_DIR)/include 
GTEST_LIBS ?= -lgtest
endif
endif

ifeq "$(LINUX_DIST)" "suse"
ifeq "$(LINUX_REV)" "11"   
GTEST_DIR ?= /usr
GTEST_LIB_DIR ?= $(GTEST_DIR)/lib  
GTEST_INCLUDE_DIR ?= $(GTEST_DIR)/include 
GTEST_LIBS ?= -lgtest
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14
ifeq "$(LINUX_REV)" "14"
GTEST_DIR ?= /usr
GTEST_LIB_DIR ?= $(GTEST_DIR)/lib  
GTEST_INCLUDE_DIR ?= $(GTEST_DIR)/include 
GTEST_LIBS ?= -lgtest
endif

# RHEL 5
ifeq "$(LINUX_REV)" "5"
GTEST_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
GTEST_LIBS ?= NOT SUPPLIED ON THIS PLATFORM
endif

endif

endif

endif

# Qt 4 (http://qt.nokia.com/products/)
######################################

ifdef WITH_QT

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
endif

ifeq "$(LINUX_DIST)" "slackware"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14 puts Qt in a subdir in /usr/lib
ifeq "$(LINUX_REV)" "14"
QT_DIR ?= /usr/lib/qt4
QT_INCLUDE_DIR ?= /usr/include
QT_LIB_DIR ?= /usr/lib
endif

ifeq "$(LINUX_REV)" "5"
QT_DIR ?= /usr/lib/qt4
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
endif

endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "squeeze/sid"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include/qt4
QT_LIB_DIR ?= $(QT_DIR)/lib
endif

ifeq "$(LINUX_REV)" "5"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include/qt4
QT_LIB_DIR ?= $(QT_DIR)/lib
endif

endif

ifeq "$(LINUX_DIST)" "suse"
ifeq "$(LINUX_REV)" "11"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
endif
endif

endif

endif

# Lua 5.1
#########

ifdef WITH_LUA

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua
endif

ifeq "$(LINUX_DIST)" "slackware"
LUA_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
LUA_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
LUA_LIBS ?= NOT SUPPLIED ON THIS PLATFORM
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include/lua5.1
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua5.1
endif
ifeq "$(LINUX_REV)" "5"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include/lua5.1
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua5.1
endif
endif

ifeq "$(LINUX_DIST)" "suse"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14
ifeq "$(LINUX_REV)" "14"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua
endif

# RHEL 5
ifeq "$(LINUX_REV)" "5"
LUA_DIR ?= /usr
LUA_INCLUDE_DIR ?= $(LUA_DIR)/include
LUA_LIB_DIR ?= $(LUA_DIR)/lib
LUA_LIBS ?= -llua
endif

endif

endif
endif

# PAM 1.1.3
###########

ifdef WITH_PAM

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif
ifeq "$(LINUX_REV)" "5"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14
ifeq "$(LINUX_REV)" "14"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam  
endif

endif

ifeq "$(LINUX_DIST)" "suse"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif
endif

# Sqlite3
#########

ifdef WITH_SQLITE3

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

endif
endif

# Postgresql
############

ifdef WITH_PGSQL

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

endif

endif
endif
