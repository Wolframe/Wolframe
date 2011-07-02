TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I. \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/gtest/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(TOPDIR)/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframe.a

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# sqlite3

ifeq ($(WITH_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

# Postgresql

ifeq ($(WITH_PGSQL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PGSQL

INCLUDE_DIRS += \
	-I$(PGSQL_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PGSQL_LIB_DIR)

INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

TEST_OBJS =

TEST_CPP_BINS = \
	dbTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

dbTest$(EXE): $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f dbTest.log test.db

local_distclean:

local_test:
	@-rm -f test.db
	@sqlite3 test.db "create table test(a integer);"
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./dbTest
	@sqlite3 test.db "drop table test;"
	@echo
	@echo
