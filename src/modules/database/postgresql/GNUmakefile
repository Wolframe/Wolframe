TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(PGSQL_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR) \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	$(PGSQL_LIBS)

NOINST_STATIC_LIB = mod_db_postgresql_lib.a

DYNAMIC_MODULE = mod_db_postgresql.so

CPP_OBJS = \
	PostgreSQLconfig.o \
	PostgreSQLconstructor.o \
	PostgreSQL.o \
	PostgreSQLtransactionExecStatemachine.o \
	PostgreSQLbindParam.o \
	PostgreSQLprogram.o \
	PostgreSQLmodule.o

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

