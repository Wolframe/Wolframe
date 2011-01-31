TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(LUA_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(LUA_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/src/libsmerp.a \
	$(LUA_LIBS) -ldl
# openssl

ifdef WITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS = \
	appConfig.o \
	configFile.o \
	luaHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	smerp-lua$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

smerplogger.so: smerplogger.sho++ $(TOPDIR)/src/logBackend.sho++ $(TOPDIR)/src/logLevel.sho++
	$(CCPP_LINK) -shared -o $@ $(LDFLAGS) $?

local_all: smerplogger.so

local_clean:
	-rm -f smerplogger.sho++ smerplogger.so

local_distclean:

local_install:

localhost_test:

run: $(CPP_BINS) smerplogger.so
	@echo "Running the SMERP server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR) ./smerp-lua -c smerp-lua.conf -f
