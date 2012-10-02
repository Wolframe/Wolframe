TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk


# PostgreSQL
ifeq ($(WITH_PGSQL),1)

INCLUDE_DIRS += \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/gtest/include \
	-I$(TOPDIR)/src/modules/database/postgresql

INCLUDE_LDFLAGS += \
	$(TOPDIR)/src/modules/database/postgresql

INCLUDE_LIBS += \
	$(TOPDIR)/src/libwolframe/libwolframe.a

TEST_CPP_BINS = \
	postgresUnit$(EXE)
endif


-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:


local_test: $(TEST_CPP_BINS)

local_longtest:

