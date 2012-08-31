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

LOCAL_STATIC_LIB = \
	libwolframe.a

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	wolframedCommandLine.o \
	wolfilterCommandLine.o \
	loadModules.o \
	serviceConfig.o \
	serviceBanner.o \
	appConfig.o \
	protocol_ioblocks.o \
	langbind_appObjects.o \
	langbind_appGlobalContext.o \
	langbind_appConfig_struct.o \
	langbind_appConfig.o \
	langbind_iostreamfilter.o \
	cmdbind_protocolCommandHandler.o \
	cmdbind_lineCommandHandler.o \
	cmdbind_execCommandHandler.o \
	cmdbind_ioFilterCommandHandler.o \
	cmdbind_doctypeFilterCommandHandler.o \
	cmdbind_directmapCommandHandler.o \
	filter_filterbase.o \
	filter_doctype.o \
	filter_typingfilter.o \
	filter_textwolf_filter.o \
	filter_char_filter.o \
	filter_line_filter.o \
	filter_token_filter.o \
	filter_tostringfilter.o \
	filter_ptreefilter.o \
	serialize_struct_filtermapBase.o \
	serialize_struct_filtermapParseStack.o \
	serialize_struct_filtermapParse.o \
	serialize_struct_filtermapSerializeStack.o \
	serialize_struct_filtermapSerialize.o \
	serialize_ddl_filtermapSerialize.o \
	serialize_ddl_filtermapParse.o \
	serialize_mapContext.o \
	ddl_atomicType.o \
	ddl_structType.o \
	ddl_structTypeBuild.o \
	ddl_compilerInterface.o \
	ddl_compiler_simpleFormCompiler.o \
	database_processor.o \
	prnt_pdfPrinterVariable.o \
	prnt_pdfPrinterMethod.o \
	prnt_pdfPrinterExpression.o \
	prnt_pdfPrinterDocument_libhpdf.o \
	prnt_pdfPrinter.o \
	types_allocators.o \
	types_bcdArithmetic.o \
	types_dateArithmetic.o \
	acceptor.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o \
	procProvider.o

#LUA
ifeq ($(WITH_LUA),1)
LOCAL_STATIC_LIB_OBJS += \
	filter_luafilter.o \
	langbind_luaObjects.o \
	langbind_luaGetFunctionClosure.o \
	langbind_luaDebug.o \
	langbind_luaException.o \
	langbind_luaBcdNumber.o \
	langbind_luaDateTime.o \
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

wolfilter$(EXE): libwolframe

wolfilter$(EXE): wolfilter.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)
	@rm -f primary.db

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
