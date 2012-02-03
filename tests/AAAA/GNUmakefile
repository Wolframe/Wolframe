TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system \
	$(LIBS_DL)

CPP_OBJS =

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# PAM

ifeq ($(WITH_PAM),1)

INCLUDE_CXXFLAGS = \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/authentication/pam

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

CPP_OBJS += \
	$(TOPDIR)/src/modules/authentication/pam/PAMAuth.o

endif

TEST_CPP_BINS = \
	authenticationTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

authenticationTest$(EXE): $(CPP_OBJS)

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./authenticationTest
	@echo
endif
