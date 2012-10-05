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
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/modules/database/sqlite3/SQLite.o \
	$(SQLITE3_LIBS) \
	$(TOPDIR)/src/modules/database/dbTransaction.o \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS =

TEST_CPP_BINS = \
	sqliteUnit$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@rm -f test.db

local_distclean:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe"

local_test: $(TEST_CPP_BINS)
	 @LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./sqliteUnit$(EXE)

local_longtest:
