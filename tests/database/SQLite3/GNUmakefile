TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk


INCLUDE_DIRS += \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(SQLITE3_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/sqlite3 \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS += \
	-L$(BOOST_LIB_DIR) \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system \
	-lboost_filesystem \
	$(SQLITE3_LIBS) \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/gtest/gtest-all.o

CPP_OBJS = \
	$(TOPDIR)/src/modules/database/sqlite3/SQLite.o \
	$(TOPDIR)/src/modules/database/dbTransaction.o

TEST_CPP_BINS = \
	sqliteUnit$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
#$(TEST_CPP_BINS)

local_longtest:
