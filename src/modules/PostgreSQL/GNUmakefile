TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	$(PGSQL_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	$(PGSQL_LIBS)

DYNAMIC_LIB = mod_db_postgresql.so
DYNAMIC_LIB_MAJOR = 0     
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0 

OBJS =

#	database/PostgreSQLconfig.o
#	database/PostgreSQL.o
#	database/PostgreSQLmodule.o

# ??
#CPP_BINS = \
#	postgresql$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(libdir)/modules
	$(INSTALL) -m 0775 $(DYNAMIC_LIB) $(libdir)/modules

local_test:
