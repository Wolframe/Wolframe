TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database"

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:

local_distclean:

# On Unix disabled, see issue #106
local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
#	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./checkConfig.sh $(EXPECT)
endif
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
