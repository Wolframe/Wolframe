TOPDIR = ../..

SUBDIRS = expect

-include $(TOPDIR)/makefiles/gmake/platform.mk


# openssl
ifdef WITH_SSL

endif

# pam
ifdef WITH_PAM

endif

TEST_OBJS =

TEST_CPP_BINS =


include $(TOPDIR)/makefiles/gmake/sub.mk

logTest: $(TOPDIR)/src/libwolframe.a
authTest: $(TOPDIR)/src/libwolframe.a

ifdef WITH_GTEST
PECHO_GTEST_OBJS=$(TOPDIR)/examples/pecho/pechoHandler.o
pechoHandler-gTest$(EXE): pechoHandler-gTest.o $(PECHO_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) $(PECHO_GTEST_OBJS) $(OBJS) $< $(LIBS)

MTPROC_GTEST_OBJS = $(TOPDIR)/examples/mtproc/mtprocHandler.o \
		$(TOPDIR)/examples/mtproc/implementation.o \
		$(TOPDIR)/examples/mtproc/implementation_c.o \
		$(TOPDIR)/examples/mtproc/dispatcher.o
mtprocHandler-gTest$(EXE): mtprocHandler-gTest.o $(MTPROC_GTEST_OBJS) $(TOPDIR)/src/libwolframe.a
	$(CCPP_LINK) -o $@ $(LDFLAGS) $(MTPROC_GTEST_OBJS) $(OBJS) $< $(LIBS)
endif

local_all:

local_clean:

local_distclean:

local_test:
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./testNamedRetVal
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./logTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./versionTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./counterTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./resolvePathTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./singletonTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./poolTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./protocolArgumentParsing-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./pechoHandler-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(GTEST_LIB_DIR) ./mtprocHandler-gTest
	@echo
