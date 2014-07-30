TOPDIR = ../../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src/modules/database/oracle \
	-I$(BOOST_INCLUDE_DIR) \
	$(ORACLE_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(BOOST_LIB_DIR) \
	-L$(ORACLE_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_filesystem \
	$(TOPDIR)/src/modules/database/oracle/mod_db_oracle_lib.a \
	$(ORACLE_LIBS) \
	-lwolframe_database \
	-lwolframe_serialize \
	-lwolframe_langbind

DYNAMIC_MODULE = \
	mod_db_oracletest.so

CPP_OBJS = \
	OracleTest.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
