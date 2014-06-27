TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database"

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:
	@rm -f primary.db

local_distclean:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton/:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/clients/lib"

local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
ifeq ($(WITH_EXAMPLES),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testEcho.ex
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testPecho.ex
endif
endif
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
