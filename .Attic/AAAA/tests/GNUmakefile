TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/src/libwolframe.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_system

TEST_OBJS =

TEST_CPP_BINS = \
	authTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthTextFileUnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthTextFileWrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthTextFileOk
ifeq ($(WITH_SQLITE3),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthDbSqlite3UnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthDbSqlite3WrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthDbSqlite3Ok
endif
ifeq ($(WITH_SASL),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslWrongMech
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslAnonymousOk
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslPlainUnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslPlainWrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslPlainOk
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslLoginUnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslLoginWrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslLoginOk
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslDigestMd5UnknownUser
#	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslDigestMd5Ok
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5UnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5WrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5Ok
endif
endif
