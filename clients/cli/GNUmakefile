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

local_test:
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe $(EXPECT) ./testCli
endif

run:
	@./wolfcli localhost 7661

runssl:
	@./wolfcli -S --CA-cert-file=CA.cert.pem --client-cert-file=client.crt --client-cert-key=client.key localhost 7961
