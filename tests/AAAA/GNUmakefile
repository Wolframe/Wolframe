TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

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

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

# pam

ifeq ($(WITH_PAM),1)

INCLUDE_CPPFLAGS += \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

endif

# sasl

ifeq ($(WITH_SASL),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SASL

INCLUDE_DIRS += \
	-I$(SASL_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS += \
	$(SASL_LIBS)

endif

# sqlite3

ifeq ($(WITH_SQLITE3),1)

ifeq ($(WITH_LOCAL_SQLITE3),0)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(SQLITE3_INCLUDE_DIR)

INCLUDE_LDFLAGS += \
	-L$(SQLITE3_LIB_DIR)

INCLUDE_LIBS += \
	$(SQLITE3_LIBS)

endif

ifeq ($(WITH_LOCAL_SQLITE3),1)

INCLUDE_CPPFLAGS += \
	-DWITH_SQLITE3

INCLUDE_DIRS += \
	-I$(TOPDIR)/sqlite3

endif

endif

TEST_OBJS =

TEST_CPP_BINS = \
	authTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

authTest$(EXE): $(TOPDIR)/src/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o
	-rm -f sasl.db

local_distclean:

sasl.db:
	echo "xx" | $(SASL_DIR)/sbin/saslpasswd2 -f sasl.db -p test

local_test: sasl.db $(TEST_CPP_BINS)
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
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslDigestMd5Ok
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5UnknownUser
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5WrongPass
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) $(EXPECT) ./checkAuthSaslCramMd5Ok
endif
endif
