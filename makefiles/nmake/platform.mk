# Sets Windows specific variables
#
# requires:
# - 
#
# provides:
# - PLATFORM_SDK_DIR: the location of the Windows Platform SDK, used
#   for compiling anything from the Win32 API into a binary/library
#
# author: Andreas Baumann, abaumann at yahoo dot com

# The location of the Windows Platform SDK
PLATFORM_SDK_DIR = C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2
#PLATFORM_SDK_DIR = C:\Programme\Microsoft Platform SDK for Windows Server 2003 R2

# TODO: maybe we have to probe certain things like the version
# of the visual studio or the availability of certain addons
# like Windows Unix Services, MOSS stuff or SDKs

#PLATFORM =		$(shell $(TOPDIR)/makefiles/gmake/guess_env --platform $(CC))
#OS_MAJOR_VERSION =	$(shell $(TOPDIR)/makefiles/gmake/guess_env --os-major-version $(CC))
#OS_MINOR_VERSION =	$(shell $(TOPDIR)/makefiles/gmake/guess_env --os-minor-version $(CC))
#COMPILER =		$(shell $(TOPDIR)/makefiles/gmake/guess_env --compiler $(CC))

#PLATFORM_COMPILE_FLAGS = \
#			-D$(PLATFORM) \
#			-DOS_MAJOR_VERSION=$(OS_MAJOR_VERSION) \
#			-DOS_MINOR_VERSION=$(OS_MINOR_VERSION)

# Boost
#######

# base dir of boost
BOOST_DIR=C:\Program Files\boost\boost_1_42
#BOOST_DIR=C:\Programme\boost\boost_1_42

# check if we have the tagged or untagged version
#BOOST_TAG ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_boost --tagged $(BOOST_DIR))
