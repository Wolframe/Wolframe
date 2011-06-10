TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:

local_distclean:

local_test:
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./testEcho
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./testPecho
endif
