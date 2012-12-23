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
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	$(TOPDIR)/gtest/gtest-all.o \
	$(PGSQL_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS = \
	PostgreSQLModuleFiles.o

TEST_CPP_BINS = \
	postgresUnit$(EXE)


LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"
DB_EXISTS = $(shell PGPASSWORD="wolfpwd" psql -h localhost -U wolfusr -d wolframe -c "" >/dev/null 2>&1; echo $$?)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(DB_EXISTS),0)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./postgresUnit$(EXE)
else
	@echo "No properly configured database for tests"
endif

local_longtest:
