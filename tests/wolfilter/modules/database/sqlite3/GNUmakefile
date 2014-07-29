TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src/modules/database/sqlite3 \
	-I$(BOOST_INCLUDE_DIR) \
	$(SQLITE3_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	-lboost_filesystem \
	$(SQLITE3_LIBS) \
	$(TOPDIR)/src/modules/database/sqlite3/mod_db_sqlite3_lib.a

DYNAMIC_MODULE = \
	mod_db_sqlite3test.so

CPP_OBJS = \
	SQLiteTest.o \

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
