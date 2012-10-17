TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options -lboost_system

# openssl

ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)

endif

OBJS =

CPP_BINS = \
	wolfcli$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:
	-rm -f primary.db

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(DESTDIR)$(bindir)

local_uninstall:
	-rm -f $(DESTDIR)$(bindir)/wolfcli(EXE)
	-rmdir $(DESTDIR)$(bindir)

local_test:
ifeq ($(WITH_EXPECT),1)
ifneq "$(PLATFORM)" "NETBSD"
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database $(EXPECT) ./testCli
endif
endif

run:
	@./wolfcli 127.0.0.1 7661

runssl:
	@./wolfcli -S --CA-cert-file=certs/CAclient.cert.pem \
		--client-cert-file=certs/client.crt \
		--client-cert-key=private/client.key \
		127.0.0.1 7961
