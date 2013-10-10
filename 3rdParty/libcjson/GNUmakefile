TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS =

INCLUDE_LDFLAGS =

INCLUDE_LIBS =

STATIC_LIB = libcjson.a

OBJS = \
	cJSON.o

TEST_BINS = \
	testit$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

# override all flags here, not just CFLAGS, compiler.mk
# cjson doesn't compile cleany, and we need -fPIC to have
# the possibility to link the code into a dynamic module
ALL_CFLAGS = $(OPTFLAGS) -I. $(SO_COMPILE_FLAGS)

local_all:

local_clean:
	@-rm -rf *.RES

local_distclean:

DIST_HEADERS = \
	cJSON.h

local_install:
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/cjson
	$(INSTALL) -m 644 $(DIST_HEADERS) $(DESTDIR)$(includedir)/wolframe/cjson

local_uninstall:
	-rm -rf $(DESTDIR)$(includedir)/wolframe/cjson/*
	-rmdir $(DESTDIR)$(includedir)/wolframe/cjson
	-rmdir $(DESTDIR)$(includedir)/wolframe

local_test: $(TEST_BINS)
ifeq ($(RUN_TESTS),1)
	@-./testit > testit.RES
	@diff testit.RES testit.MUST
endif
