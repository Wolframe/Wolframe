TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_functions:$(TOPDIR)/src/libwolframe_database:$(LIBXML2_LIB_DIR)"

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:

local_distclean:

# Aba, test hangs on FreeBSD, Windows is out of reach!
local_test:
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./checkConfig.sh $(EXPECT)
endif
