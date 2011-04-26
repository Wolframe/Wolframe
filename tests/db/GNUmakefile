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

# gtest

ifeq ($(WITH_GTEST),1)
INCLUDE_CPPFLAGS += \
	-I$(GTEST_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(GTEST_LIB_DIR)

INCLUDE_LIBS += \
	$(GTEST_LIBS)
endif

TEST_OBJS =

TEST_CPP_BINS =

ifeq ($(WITH_GTEST),1)
TEST_CPP_BINS += \
	sqlite3ppTest$(EXE) \
	sdsqliteTest$(EXE)
endif

sqlite3ppTest$(EXE): $(TOPDIR)/src/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(WITH_SQLITE3),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./sdsqliteTest
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./sqlite3ppTest
endif
