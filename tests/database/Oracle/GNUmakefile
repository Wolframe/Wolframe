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
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	$(ORACLE_LIB_DIRS)

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_database \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(ORACLE_LIBS) \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system -lboost_filesystem

CPP_OBJS = \
	OracleModuleFiles.o

CPP_BINS = \
	dbPing$(EXE)

TEST_CPP_BINS = \
	oracleUnit$(EXE)


LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(ORACLE_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
local_test: $(TEST_CPP_BINS)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./dbPing andreasbaumann.dyndns.org wolfusr wolfpwd orcl; \
		if test $$? -eq 0; then \
			LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./oracleUnit$(EXE) ; \
		else \
			echo "No properly configured database for tests" ; \
		fi

local_longtest:

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
