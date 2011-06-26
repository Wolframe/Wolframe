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

# set up defaults for the build switches
WITH_SSL ?= 0
WITH_LUA ?= 0
WITH_SASL ?= 0
WITH_PAM ?= 0
WITH_SQLITE3 ?= 0
WITH_PGSQL ?= 0
WITH_LIBXML2 ?= 0
WITH_QT ?= 0
WITH_EXAMPLES ?= 1

# variables guessed by 'guess_env'
PLATFORM ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --platform "$(CC)" "$(CURDIR)" $(TOPDIR))
OS_MAJOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-major-version "$(CC)" "$(CURDIR)" $(TOPDIR))
OS_MINOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-minor-version "$(CC)" "$(CURDIR)" $(TOPDIR))
COMPILER ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --compiler "$(CC)" "$(CURDIR)" $(TOPDIR))
ifeq "$(PLATFORM)" "LINUX" 
LINUX_DIST ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-dist "$(CC)" "$(CURDIR)" $(TOPDIR))
LINUX_REV ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-rev "$(CC)" "$(CURDIR)" $(TOPDIR))
endif

# set library path on Intel/AMD
ifeq "$(ARCH)" "x86"
ifeq "$(LINUX_DIST)" "arch"
LIBDIR=/usr/lib32
else
LIBDIR=/usr/lib
endif
else
LIBDIR=/usr/lib
endif
ifeq "$(ARCH)" "x86_64"
ifeq "$(LINUX_DIST)" "arch"
LIBDIR=/usr/lib
else
LIBDIR=/usr/lib64
endif
else
LIBDIR=/usr/lib
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

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
ifeq "$(LINUX_REV)" "5"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
ifeq "$(LINUX_REV)" "6"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14
ifeq "$(LINUX_REV)" "14"
XSLT_VERSION ?= $(shell rpm -q --queryformat '%{VERSION}' docbook-style-xsl)
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# Fedora 14
ifeq "$(LINUX_REV)" "15"
XSLT_VERSION ?= $(shell rpm -q --queryformat '%{VERSION}' docbook-style-xsl)
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# RHEL4
ifeq "$(LINUX_REV)" "4"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
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

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
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

ifeq "$(LINUX_REV)" "6"
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

endif

ifeq "$(LINUX_DIST)" "suse"

ifeq "$(LINUX_REV)" "11.4"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

ifeq "$(LINUX_REV)" "12.1"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

endif

# SUSE Linux Enterprise
ifeq "$(LINUX_DIST)" "sles"

ifeq "$(LINUX_REV)" "11"
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

# Fedora 15
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "15"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?= -mt
endif
endif

# RHEL4
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "4"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# RHEL5
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "5"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# RHEL6
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "6"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
BOOST_DIR ?= /usr/local
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# OpenSSL
#########

ifeq ($(WITH_SSL),1)

ifeq "$(PLATFORM)" "LINUX"

OPENSSL_LIBS ?= -lssl -lcrypto

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
OPENSSL_LIBS ?= -lssl -lcrypto
endif
endif

endif

# Lua 5.1
#########

ifeq ($(WITH_LUA),1)

ifeq "$(PLATFORM)" "LINUX"
LUA_PLATFORM_CFLAGS = -DLUA_USE_LINUX
LUA_PLATFORM_LDFLAGS = -ldl
endif

ifeq "$(PLATFORM)" "FREEBSD"
LUA_PLATFORM_CFLAGS = -DLUA_USE_POSIX
LUA_PLATFORM_LDFLAGS =
endif

endif

# Qt 4 (http://qt.nokia.com/products/)
######################################

ifeq ($(WITH_QT),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

ifeq "$(LINUX_DIST)" "slackware"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 14 puts Qt in a subdir in /usr/lib
ifeq "$(LINUX_REV)" "14"
QT_DIR ?= $(LIBDIR)/qt4
QT_INCLUDE_DIR ?= /usr/include
QT_LIB_DIR ?= $(LIBDIR)
QT_MOC ?= $(QT_DIR)/bin/moc
endif

# Fedora 15 puts Qt in a subdir in /usr/lib
ifeq "$(LINUX_REV)" "15"
QT_DIR ?= $(LIBDIR)/qt4
QT_INCLUDE_DIR ?= /usr/include
QT_LIB_DIR ?= $(LIBDIR)
QT_MOC ?= $(QT_DIR)/bin/moc
endif

# RHEL 5
ifeq "$(LINUX_REV)" "5"
QT_DIR ?= /usr/lib/qt4
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

# RHEL 6
ifeq "$(LINUX_REV)" "6"
QT_DIR ?= /usr/lib/qt4
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, 6.0
ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "squeeze/sid"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include/qt4
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

ifeq "$(LINUX_REV)" "5"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include/qt4
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

ifeq "$(LINUX_REV)" "6"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include/qt4
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

endif

ifeq "$(LINUX_DIST)" "sles"
ifeq "$(LINUX_REV)" "11"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif
endif

ifeq "$(LINUX_DIST)" "suse"

ifeq "$(LINUX_REV)" "11.4"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

ifeq "$(LINUX_REV)" "12.1"
QT_DIR ?= /usr
QT_INCLUDE_DIR ?= $(QT_DIR)/include
QT_LIB_DIR ?= $(QT_DIR)/lib
QT_MOC ?= $(QT_DIR)/bin/moc
endif

endif

endif

endif

# PAM 1.1.3
###########

ifeq ($(WITH_PAM),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_DIST)" "slackware"
PAM_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_LIBS ?= NOT SUPPLIED ON THIS PLATFORM
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
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
ifeq "$(LINUX_REV)" "6"
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

# Fedora 15
ifeq "$(LINUX_REV)" "15"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# RHEL4
ifeq "$(LINUX_REV)" "4"
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

# RHEL6
ifeq "$(LINUX_REV)" "6"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam  
endif

endif

ifeq "$(LINUX_DIST)" "sles"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_DIST)" "suse"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
PAM_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_INCLUDE_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_LIB_DIR ?= NOT SUPPLIED ON THIS PLATFORM
PAM_LIBS ?= NOT SUPPLIED ON THIS PLATFORM
endif
endif

endif

# Cyrus SASL2
#############

ifeq ($(WITH_SASL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_DIST)" "slackware"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
ifeq "$(LINUX_REV)" "5"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
ifeq "$(LINUX_REV)" "6"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL4
ifeq "$(LINUX_REV)" "4"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Fedora 14
ifeq "$(LINUX_REV)" "14"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Fedora 15
ifeq "$(LINUX_REV)" "15"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

ifeq "$(LINUX_DIST)" "sles"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_DIST)" "suse"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
SASL_DIR ?= /usr/local
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
endif

endif

# Sqlite3
#########

ifeq ($(WITH_SQLITE3),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_DIST)" "slackware"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib  
SQLITE3_LIBS ?= -lsqlite3
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifeq "$(LINUX_REV)" "5"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifeq "$(LINUX_REV)" "6"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL4
ifeq "$(LINUX_REV)" "4"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# Fedora 14
ifeq "$(LINUX_REV)" "14"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# Fedora 15
ifeq "$(LINUX_REV)" "15"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

ifeq "$(LINUX_DIST)" "sles"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_DIST)" "suse"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
SQLITE3_DIR ?= /usr/local
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
endif

endif

# Postgresql
############

ifeq ($(WITH_PGSQL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_DIST)" "slackware"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "squeeze/sid"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
ifeq "$(LINUX_REV)" "5"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
ifeq "$(LINUX_REV)" "6"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL4
ifeq "$(LINUX_REV)" "4"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# Fedora 14
ifeq "$(LINUX_REV)" "14"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

# Fedora 15
ifeq "$(LINUX_REV)" "15"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

endif

ifeq "$(LINUX_DIST)" "sles"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/pgsql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_DIST)" "suse"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/pgsql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
PGSQL_DIR ?= /usr/local
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif
endif

endif

# libxml2
#########

ifeq ($(WITH_PGSQL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Ubuntu 10.04 TLS, 10.10, Debian 5.0, Debian 6.0
ifeq "$(LINUX_DIST)" "debian"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
ifeq "$(LINUX_REV)" "squeeze/sid"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
ifeq "$(LINUX_REV)" "5"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
ifeq "$(LINUX_REV)" "6"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL4
ifeq "$(LINUX_REV)" "4"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib  
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib  
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib  
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Fedora 14
ifeq "$(LINUX_REV)" "14"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib  
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Fedora 15
ifeq "$(LINUX_REV)" "15"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib  
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

ifeq "$(LINUX_DIST)" "sles"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_DIST)" "suse"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

endif

# Expect (for testing)
######################

ifeq ($(WITH_EXPECT),1)

ifeq "$(PLATFORM)" "LINUX"
EXPECT = /usr/bin/expect
endif

ifeq "$(PLATFORM)" "FREEBSD"
EXPECT = /usr/local/bin/expect
endif

endif
