TOPDIR = ../../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBGMP_INCLUDE_DIRS) \
	-I$(TOPDIR)/lua/src

INCLUDE_LDFLAGS = \
	$(LIBGMP_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBGMP_LIBS) \
	$(TOPDIR)/lua/src/liblua.a

DYNAMIC_MODULE = \
	c-gmp.so

OBJS = \
	lgmp.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) $(TOPDIR)/lua/src/lua ./test.lua 4 5
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) $(TOPDIR)/lua/src/lua ./test.lua 424234238490238409238402380482304 52342342342342342342342323
	@echo

local_longtest:
