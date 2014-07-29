TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src/modules/database/postgresql \
	-I$(BOOST_INCLUDE_DIR) \
	$(PGSQL_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(BOOST_LIB_DIR) \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	-lboost_filesystem \
	$(PGSQL_LIBS) \
	$(TOPDIR)/src/modules/database/postgresql/mod_db_postgresql_lib.a

DYNAMIC_MODULE = \
	mod_db_postgresqltest.so

CPP_OBJS = \
	PostgreSQLTest.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
