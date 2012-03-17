TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

include $(TOPDIR)/makefiles/gmake/sub.mk

configTest: $(TOPDIR)/src/wolframed

local_all:

local_clean:

local_distclean:

# Aba, test hangs on FreeBSD, Windows is out of reach!
local_test:
ifeq ($(RUN_TESTS),1)
ifneq "$(PLATFORM)" "FREEBSD"
ifneq "$(PLATFORM)" "OPENBSD"
ifneq "$(PLATFORM)" "NETBSD"
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/src/libwolframe ./checkConfig.sh $(EXPECT)
endif
endif
endif
endif
