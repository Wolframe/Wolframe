TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	$(PGSQL_INCLUDE_DIRS) \
	-I$(TOPDIR)/src/modules/database/postgresql \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/wtest \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_database \
	-lwolftest \
	$(TOPDIR)/src/libwolframed.a \
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

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	@-rm -f gtestReport.txt
	@-rm -f postgresUnit*.xml

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./dbPing localhost wolfusr wolfpwd wolframe; \
		if test $$? -eq 0; then \
			LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./postgresUnit$(EXE) ; \
		else \
			echo "No properly configured database for tests" ; \
		fi
endif

local_longtest:

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
