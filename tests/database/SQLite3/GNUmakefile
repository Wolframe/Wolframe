TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk


INCLUDE_DIRS = \
	-I . \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(SQLITE3_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/sqlite3 \
	-I$(TOPDIR)/3rdParty/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	$(SQLITE3_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(SQLITE3_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS = \
	sqliteModuleFiles.o

TEST_CPP_BINS = \
	sqliteUnit$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"


local_all:

local_clean:
	@rm -f test.db garbage.db

local_distclean:

local_test: $(TEST_CPP_BINS)
	@rm -f test.db garbage.db
	@dd if=/dev/urandom of=garbage.db bs=4096 count=1
	@$(SQLITE3) test.db < testDB.sql
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./sqliteUnit$(EXE)

local_longtest:
