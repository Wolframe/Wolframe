TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include -I$(TOPDIR)/src \
	-I$(TOPDIR)/3rdParty/lua/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	-lboost_date_time \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/3rdParty/lua/src/liblua.a \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_DIRS += \
	$(OPENSSL_INCLUDE_DIRS)
	
INCLUDE_LDFLAGS += \
	$(OPENSSL_LIB_DIRS)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

CPP_OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	luaConfig.o \
	luaHandler.o \
	$(TOPDIR)/src/posixMain.o

CPP_BINS = \
	luad$(EXE)

#luad$(EXE): $(TOPDIR)/src/posixMain.o $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe"

local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testLua
endif
endif

run: $(CPP_BINS)
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./luad -c luad.conf -f -d DATA
