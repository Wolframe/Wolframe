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
#
# author: Andreas Baumann, abaumann at yahoo dot com

-include $(TOPDIR)/makefiles/gmake/platform.mk.vars

PLATFORM ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --platform $(CC) "$(CURDIR)" $(TOPDIR))
OS_MAJOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-major-version $(CC) "$(CURDIR)" $(TOPDIR))
OS_MINOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-minor-version $(CC) "$(CURDIR)" $(TOPDIR))
COMPILER ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --compiler $(CC) "$(CURDIR)" $(TOPDIR))

# platform specific flags
#########################

PLATFORM_COMPILE_FLAGS = \
			-D$(PLATFORM) \
			-DOS_MAJOR_VERSION=$(OS_MAJOR_VERSION) \
			-DOS_MINOR_VERSION=$(OS_MINOR_VERSION)

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

# command line parser generator gengetopt
########################################

# location of gengetopt (default: in the path)
GENGETOPT=gengetopt

# some platform either have no getopt/getopt_long or a broken one, so
# gengetopt can include its own one

GENGETOPT_INCLUDE_GETOPT = 

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "8"
GENGETOPT_INCLUDE_GETOPT =  --include-getopt
endif
endif

endif

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

XSLTPROC = xsltproc
