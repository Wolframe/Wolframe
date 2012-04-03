TOPDIR = ../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I$(TOPDIR)/lua/src

INCLUDE_LDFLAGS =

INCLUDE_LIBS = \
	$(TOPDIR)/lua/src/liblua.a

DYNAMIC_MODULE = \
	c-testmod.so

OBJS = \
	c-testmod.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
	@LD_LIBRARY_PATH=$(LIBGMP_LIB_DIR) $(TOPDIR)/lua/src/lua ./test.lua World 47 3.1415
	@echo

local_longtest:
