TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(BOOST_LIB_DIR) \
	$(SQLITE3_LIB_DIRS)

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(SQLITE3_INCLUDE_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lboost_filesystem \
	$(SQLITE3_LIBS)

NOINST_STATIC_LIB = mod_db_sqlite3_lib.a

DYNAMIC_MODULE = mod_db_sqlite3.so

CPP_OBJS = \
	SQLite.o \
	SQLiteConfig.o \
	SQLiteTransactionExecStatemachine.o \
	SQLiteStatement.o \
	mod_db_sqlite3.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:
