TOPDIR = ..

SUBDIRS =


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
	@-./testit > test.RES
	@diff test.RES test.MUST
endif
