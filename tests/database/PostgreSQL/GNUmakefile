TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(PGSQL_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/postgresql \
	-I$(TOPDIR)/3rdParty/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfshared \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lwfshared \
	-lwolframe \
	-lwolframe_database \
	$(TOPDIR)/src/libwolframe.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(PGSQL_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS = \
	PostgreSQLModuleFiles.o

CPP_BINS = \
	dbPing$(EXE)

TEST_CPP_BINS = \
	postgresUnit$(EXE) \

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/src/libwfshared:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./dbPing localhost wolfusr wolfpwd wolframe; \
		if test $$? -eq 0; then \
			LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./postgresUnit$(EXE) ; \
		else \
			echo "No properly configured database for tests" ; \
		fi

local_longtest:

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
