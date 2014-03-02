TOPDIR = ..

SUBDIRS = libwolframe libwfshared libwolframe_serialize libwolframe_database libwolframe_langbind modules

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfshared \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwfshared \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
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

LOCAL_STATIC_LIB = \
	libwolframe.a

PROVIDER_OBJS = \
	provider/Authentication.o \
	provider/Authorization.o \
	provider/Audit.o \
	provider/AAAAproviderImpl.o \
	provider/AAAAproviderConfig.o \
	provider/DBproviderImpl.o \
	provider/DBproviderConfig.o \
	provider/procProvider.o \
	provider/procProviderImpl.o

LOCAL_STATIC_LIB_OBJS = \
	$(PROVIDER_OBJS) \
	wolframedCommandLine.o \
	wolfilterCommandLine.o \
	wolfilterIostreamFilter.o \
	wolfwizardCommandLine.o \
	mainConnectionHandler.o \
	interfaceCommandHandler.o \
	loadModules.o \
	moduleDirectory.o \
	serviceConfig.o \
	serviceBanner.o \
	appConfig.o \
	prgbind_transactionProgram.o \
	prgbind_normalizeProgram.o \
	prgbind_ddlProgram.o \
	prgbind_programLibrary.o \
	jobScheduler.o \
	acceptor.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	server.o

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
	wolfilter$(EXE) \
	wolfwizard$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

# # set default module load directory for configuration option 'directory' in 'LoadModules'
ifdef DEFAULT_MODULE_LOAD_DIR
INCLUDE_CXXFLAGS += -DDEFAULT_MODULE_LOAD_DIR=$(DEFAULT_MODULE_LOAD_DIR)
endif

modules: libwolframe libwolframe_serialize libwolframe_langbind
libwolframe_serialize: libwolframe
libwolframe_langbind: libwolframe libwolframe_serialize

wolframed$(EXE): libwolframe libwolframe_serialize libwolframe_langbind

wolframed$(EXE): wolframed.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

wolfilter$(EXE): libwolframe libwolframe_serialize libwolframe_langbind

wolfilter$(EXE): wolfilter.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

wolfwizard$(EXE): libwolframe libwolframe_serialize libwolframe_langbind

wolfwizard$(EXE): wolfwizard.o $(OBJS) $(CPP_OBJS)
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
	$(INSTALL) -m 0775 wolframed$(EXE) $(DESTDIR)$(sbindir)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfilter$(EXE) $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfwizard$(EXE) $(DESTDIR)$(bindir)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sysconfdir)/wolframe
	$(INSTALL) -m 0755 wolframe.conf $(DESTDIR)$(sysconfdir)/wolframe

local_uninstall:
	@-rm -f $(DESTDIR)$(bindir)/wolfwizard$(EXE)
	@-rm -f $(DESTDIR)$(bindir)/wolfilter$(EXE)
	@-rmdir $(DESTDIR)$(bindir)
	@-rm -f $(DESTDIR)$(sbindir)/wolframed$(EXE)
	@-rmdir $(DESTDIR)$(sbindir)
	@-rm -f $(DESTDIR)$(sysconfdir)/wolframe/wolframe.conf
	@-rmdir $(DESTDIR)$(sysconfdir)/wolframe
	@-rmdir $(DESTDIR)$(sysconfdir)

local_test:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database"

ifeq ($(WITH_ICU),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(ICU_LIB_DIR)"
endif

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

run:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -d DATA -c wolframe.conf -f

check:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -t

print:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -p

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
