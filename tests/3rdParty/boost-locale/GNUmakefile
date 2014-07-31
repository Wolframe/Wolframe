TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I. \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/wtest \
	$(ICU_LIB_DIRS)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	-lwolftest \
	-lboost_locale$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(ICU_LIBS)

TEST_OBJS =

TEST_CPP_BINS = \
	boostLocaleTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(ICU_LIB_DIR)"

# for now we test whether the locale is installed, if not we don't
# execute the test. In the future we should make sure the test environment
# has the necessary locales installed (not necessarily OBS, as this is 
# a build and not a system test environment)
ifeq "$(PLATFORM)" "LINUX"
TEST_LOCALE=$(shell locale -a | grep en_US.utf8)
endif
ifeq "$(PLATFORM)" "FREEBSD"
TEST_LOCALE=$(shell locale -a | grep en_US.UTF-8)
endif
ifeq "$(PLATFORM)" "NETBSD"
# disabled, breaks, see #59 and #127
TEST_LOCALE=
#TEST_LOCALE=$(shell locale -a | grep en_US.UTF-8)
endif
ifeq "$(PLATFORM)" "SUNOS"
# disabled, breaks, see #127
TEST_LOCALE=
#TEST_LOCALE=$(shell locale -a | grep en_US.UTF-8)
endif

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
ifneq ($(TEST_LOCALE),)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) LANG=$(TEST_LOCALE) ./boostLocaleTest
	@echo
endif
endif
