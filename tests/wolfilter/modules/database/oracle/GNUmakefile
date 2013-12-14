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
	-L$(BOOST_LIB_DIR) \
	-L$(ORACLE_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	-lboost_filesystem \
	$(ORACLE_LIBS)

DYNAMIC_MODULE = \
	mod_db_oracletest.so

CPP_OBJS = \
	OracleModuleFiles.o \
	OracleTest.o \
	mod_db_oracletest.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
