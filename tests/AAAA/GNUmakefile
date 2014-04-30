TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/src/modules/authentication/textfile \
	-I$(TOPDIR)/src/modules/authentication/database

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwfsingleton/libwfsingleton.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system

CPP_OBJS = \
	$(TOPDIR)/src/modules/authentication/textfile/TextFileAuth.o \
	$(TOPDIR)/src/modules/authentication/database/DBauth.o \
	$(TOPDIR)/src/modules/authentication/textfile/passwdFile.o

# PAM

ifeq ($(WITH_PAM),1)

INCLUDE_CXXFLAGS = \
	-DWITH_PAM

INCLUDE_DIRS += \
	-I$(PAM_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/authentication/pam

INCLUDE_LDFLAGS += \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS += \
	$(PAM_LIBS)

CPP_OBJS += \
	$(TOPDIR)/src/modules/authentication/pam/PAMAuth.o

endif

# sasl

ifeq ($(WITH_SASL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SASL

INCLUDE_DIRS += \
	-I$(SASL_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/authentication/sasl

INCLUDE_LDFLAGS += \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS += \
	$(SASL_LIBS)

CPP_OBJS += \
	$(TOPDIR)/src/modules/authentication/sasl/SaslAuth.o

endif

TEST_CPP_BINS = \
	authenticationTest$(EXE) \
	passwdFileTest$(EXE) \
	textFileTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk


authenticationTest$(EXE): $(CPP_OBJS)
passwdFileTest$(EXE): $(CPP_OBJS)
textFileTest$(EXE): $(CPP_OBJS)


local_all:


local_clean:
	-rm -f sasl.db

local_distclean:

sasl.db:
ifeq ($(WITH_SASL),1)
	echo "xx" | $(SASL_DIR)/sbin/saslpasswd2 -f sasl.db -p test
ifeq "$(PLATFORM)" "FREEBSD"
	mv sasl.db.db sasl.db
endif
ifeq "$(PLATFORM)" "NETBSD"
	mv sasl.db.db sasl.db
endif
endif

local_test: sasl.db $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./authenticationTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./passwdFileTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./textFileTest
	@echo
endif
