TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(ORACLE_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/oracle \
	-I$(TOPDIR)/3rdParty/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	$(ORACLE_LIB_DIRS)

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_database \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(ORACLE_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS = \
	OracleModuleFiles.o

TEST_CPP_BINS = \
	oracleUnit$(EXE)


LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"
#DB_EXISTS = $(shell PGPASSWORD="wolfpwd" psql -h localhost -U wolfusr -d wolframe -c "" >/dev/null 2>&1; echo $$?)
DB_EXISTS = 0

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(DB_EXISTS),0)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./oracleUnit$(EXE)
else
	@echo "No properly configured database for tests"
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)

local_longtest:
