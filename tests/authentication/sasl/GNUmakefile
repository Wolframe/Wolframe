TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-DWITH_SASL

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(SASL_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/authentication/sasl

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(SASL_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwfsingleton/libwfsingleton.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system \
	$(SASL_LIBS)

CPP_OBJS = \
	$(TOPDIR)/src/modules/authentication/sasl/SaslAuth.o

TEST_CPP_BINS = \
	saslTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

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

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/wtest"

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(PAM_LIB_DIR)"

local_test: sasl.db $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./saslTest
	@echo	
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)

