TOPDIR = ..

SUBDIRS = libwolframe modules

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(TOPDIR) \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lwolframe \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(LIBS_DL)

# openssl
ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)
endif

# lua
ifeq ($(WITH_LUA),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LUA

INCLUDE_DIRS += \
	-I$(TOPDIR)/lua/src

INCLUDE_LIBS += \
	$(TOPDIR)/lua/src/liblua.a \
	$(LIBS_DL)
endif

# libxml2
ifeq ($(WITH_LIBXML2),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBXML2

INCLUDE_DIRS += \
	$(LIBXML2_INCLUDE_DIRS)

INCLUDE_LDFLAGS += \
	$(LIBXML2_LIB_DIRS)

INCLUDE_LIBS += \
	$(LIBXML2_LIBS)
endif

# libgmp
ifeq ($(WITH_LIBGMP),1)
INCLUDE_CXXFLAGS += \
	-DWITH_LIBGMP
INCLUDE_LIBS += \
	$(LIBGMP_LIBS)
endif

LOCAL_STATIC_LIB = \
	libwolframe.a

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	commandLine.o \
	serviceConfig.o \
	serviceBanner.o \
	appConfig.o \
	config_descriptionBase.o \
	protocol_ioblocks.o \
	langbind_appObjects.o \
	langbind_appGlobalContext.o \
	langbind_appConfig_struct.o \
	langbind_appConfig.o \
	langbind_iostreamfilter.o \
	cmdbind_commandHandler.o \
	cmdbind_lineCommandHandler.o \
	cmdbind_execCommandHandler.o \
	cmdbind_ioFilterCommandHandler.o \
	cmdbind_directmapCommandHandler.o \
	filter_filterbase.o \
	filter_serializefilter.o \
	filter_textwolf_filter.o \
	filter_char_filter.o \
	filter_line_filter.o \
	filter_token_filter.o \
	serialize_struct_filtermapBase.o \
	serialize_ddl_filtermapPrint.o \
	serialize_ddl_filtermapParse.o \
	serialize_mapContext.o \
	ddl_atomicType.o \
	ddl_structType.o \
	ddl_compilerInterface.o \
	ddl_compiler_simpleFormCompiler.o \
	types_bignum.o \
	acceptor.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o \
	procProvider.o \

#LUA
ifeq ($(WITH_LUA),1)
LOCAL_STATIC_LIB_OBJS += \
	filter_luafilter.o \
	langbind_luaObjects.o \
	langbind_luaDebug.o \
	langbind_luaBignum.o \
	cmdbind_luaCommandHandler.o
endif

#LIBXML2
ifeq ($(WITH_LIBXML2),1)
LOCAL_STATIC_LIB_OBJS += \
	filter_libxml2_filter.o
endif

#SSL
ifeq ($(WITH_SSL),1)
LOCAL_STATIC_LIB_OBJS += \
	SSLcertificateInfo.o
endif

CPP_OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	$(LOCAL_STATIC_LIB_OBJS) \
	loadModules.o \
	wolframeHandler.o \
	posixMain.o

CPP_BINS = \
	wolframed$(EXE) \
	wolfilter$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

modules: libwolframe

wolframed$(EXE): libwolframe

wolframed$(EXE): wolframed.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

wolfilter$(EXE): wolfilter.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all: $(LOCAL_STATIC_LIB) loadModules-dummy.o

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)
	@rm -f primary.db
	@rm -f loadModules-dummy.o

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sbindir)
	$(INSTALL) -m 0775 wolframed $(DESTDIR)$(sbindir)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfilter $(DESTDIR)$(bindir)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sysconfdir)/wolframe
	$(INSTALL) -m 0755 wolframe.conf $(DESTDIR)$(sysconfdir)/wolframe

local_uninstall:
	@-rm -f $(DESTDIR)$(bindir)/wolfilter
	@-rmdir $(DESTDIR)$(bindir)
	@-rm -f $(DESTDIR)$(sbindir)/wolframed
	@-rmdir $(DESTDIR)$(sbindir)
	@-rm -f $(DESTDIR)$(sysconfdir)/wolframe/wolframe.conf
	@-rmdir $(DESTDIR)$(sysconfdir)/wolframe
	@-rmdir $(DESTDIR)$(sysconfdir)

local_test:

run:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(LIBXML2_LIB_DIR):$(TOPDIR)/src/libwolframe ./wolframed -c wolframe.conf -f

check:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(LIBXML2_LIB_DIR):$(TOPDIR)/src/libwolframe ./wolframed -c wolframe.conf -t

print:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(PGSQL_LIB_DIR):$(LIBXML2_LIB_DIR):$(TOPDIR)/src/libwolframe ./wolframed -c wolframe.conf -p
