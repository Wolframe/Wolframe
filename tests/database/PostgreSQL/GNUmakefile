TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(PGSQL_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/postgresql \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/modules/database/postgresql/PostgreSQL.o \
	$(PGSQL_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS =

TEST_CPP_BINS = \
	postgresUnit$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/src/libwolframe"

local_test: $(TEST_CPP_BINS)
#	 @LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./postgresUnit$(EXE)

local_longtest:
