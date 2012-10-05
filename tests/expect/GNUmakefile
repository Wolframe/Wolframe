TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_functions:$(LIBXML2_LIB_DIR)"

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:
	@rm -f primary.db

local_distclean:

local_test:
ifeq ($(RUN_TESTS),1)
ifneq "$(PLATFORM)" "OPENBSD"
ifneq "$(PLATFORM)" "NETBSD"
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testEcho.exp
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testPecho.exp
endif
endif
endif
