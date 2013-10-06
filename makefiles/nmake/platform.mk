# Sets Windows specific variables
#
# provides:
# - BOOST_LIBRARY_TAG: how libraries are auto-tagged, for example
#   libboost_log-vc90-mt-1_42
# - OPENSSL_LIBS: libraries to link against for SSL/TLS support
# - EXPECT: location of the expect binary for automated testing

!INCLUDE $(TOPDIR)\makefiles\nmake\config.mk

# Boost
#######

!IFNDEF DEBUG
BOOST_LIBRARY_TAG=-$(BOOST_VC_VER)$(BOOST_MT)-$(BOOST_VERSION)
!ELSE
BOOST_LIBRARY_TAG=-$(BOOST_VC_VER)$(BOOST_MT)-gd-$(BOOST_VERSION)
!ENDIF

# OpenSSL
#########

!IFDEF WITH_SSL
OPENSSL_LIBS = libeay32.lib ssleay32.lib
!ENDIF

# ICU (icu4c)
#############

!IFDEF WITH_ICU

ICU_LDFLAGS = /LIBPATH:"$(ICU_LIB_DIR)"

!IFNDEF DEBUG
ICU_LIBS = icuin.lib icuuc.lib
!ENDIF

!IFDEF DEBUG
ICU_LIBS = icuind.lib icuucd.lib
!ENDIF

!ENDIF

# Tcl (for testing with Expect)
###############################

!IFDEF WITH_EXPECT
EXPECT = $(TCL_DIR)\bin\tclsh.exe
!ENDIF

# WIX Microsoft Installer (for deployment)
##########################################

CANDLE = "$(WIX_DIR)\bin\candle.exe"
LIGHT = "$(WIX_DIR)\bin\light.exe"
LIT = "$(WIX_DIR)\bin\lit.exe"
SMOKE = "$(WIX_DIR)\bin\smoke.exe"
SETUPBLD = "$(WIX_DIR)\bin\setupbld.exe"

# XSLT processor
################

XSLTPROC = $(LIBXSLT_DIR)\bin\xsltproc.exe

# Sqlite3 (always embedded)
###########################

SQLITE3 = $(TOPDIR)\sqlite3\sqlite3

# FreeImage (always embedded)
#############################


# cJSON (always embedded)
#############################

CJSON_DIR = $(TOPDIR)\libcjson


