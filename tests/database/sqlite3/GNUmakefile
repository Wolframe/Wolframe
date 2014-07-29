TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk


INCLUDE_DIRS = \
	-I . \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(SQLITE3_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/sqlite3 \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/wtest \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_langbind \
	$(SQLITE3_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwfsingleton \
	-lwolframe_database \
	-lwolframe_serialize \
	-lwolframe_langbind \
	-lwolftest \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(SQLITE3_LIBS) \
	$(TOPDIR)/src/modules/database/sqlite3/mod_db_sqlite3_lib.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

CPP_BINS = \
	createRandom$(EXE)

TEST_CPP_BINS = \
	sqliteUnit$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"


local_all:

local_clean:
	@-rm -f test.db garbage.db

local_distclean:

test.db:
	$(SQLITE3) test.db < testDB.sql

garbage.db:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./createRandom$(EXE) 4096 garbage.db

local_test: $(TEST_CPP_BINS) test.db garbage.db
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./sqliteUnit$(EXE)
endif

local_longtest:

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
