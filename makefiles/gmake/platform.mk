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

XSLTPROC = xsltproc

# Boost
#######

ifeq "$(PLATFORM)" "LINUX"
ifeq "$(LINUX_DIST)" "arch"
BOOST_DIR = /usr
BOOST_LIBRARY_TAG = -mt
endif
endif

# OpenSSL
#########

OPENSSL_LIBS = -lssl

#TODO: no fakelogger
ifdef WITH_BOOST_LOG
BOOST_LOG_LIBS=-lboost_log -lboost_log_setup
endif

# Google Unit testing gtest
###########################

ifdef WITH_GTEST
GTEST_LIBS = -lgtest
endif
