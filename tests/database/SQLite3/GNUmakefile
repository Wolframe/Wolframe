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
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	$(SQLITE3_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwfsingleton \
	-lwolframe_database \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(SQLITE3_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem \
	$(LIBS_DL) $(LIBS_LT)

CPP_OBJS = \
	sqliteModuleFiles.o

CPP_BINS = \
	createRandom$(EXE)

TEST_CPP_BINS = \
	sqliteUnit$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"


local_all:

local_clean:
	@rm -f test.db garbage.db

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
