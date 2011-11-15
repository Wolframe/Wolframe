# Sets Windows specific variables
#
# provides:
# - BOOST_LIBRARY_TAG: how libraries are auto-tagged, for example
#   libboost_log-vc90-mt-1_42
#
# author: Andreas Baumann, abaumann at yahoo dot com

!include $(TOPDIR)\makefiles\nmake\config.mk

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

# Microsoft Platform SDK
########################

# Boost
#######

BOOST_LIBRARY_TAG=-$(BOOST_VC_VER)$(BOOST_MT)-$(BOOST_VERSION)

# OpenSSL
#########

!IFDEF WITH_SSL
OPENSSL_LIBS = libeay32.lib ssleay32.lib
!ENDIF

# Tcl (for testing with Expect)
###############################

!IFDEF WITH_EXPECT
EXPECT = $(TCL_DIR)\bin\tclsh.exe
!ENDIF
