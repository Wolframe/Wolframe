TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system \
	$(TOPDIR)/src/libwolframe.a

# gtest

ifeq ($(WITH_GTEST),1)
INCLUDE_CPPFLAGS += \
	-I$(GTEST_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(GTEST_LIB_DIR)

INCLUDE_LIBS += \
	$(GTEST_LIBS)
endif

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# pam

ifeq ($(WITH_PAM),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

endif

# sasl

ifeq ($(WITH_SASL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SASL

INCLUDE_DIRS += \
	-I$(SASL_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS += \
	$(SASL_LIBS)

endif

# sqlite3

ifeq ($(WITH_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

TEST_OBJS =

TEST_CPP_BINS = \
	authTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

authTest$(EXE): $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o

local_distclean:

local_test:
