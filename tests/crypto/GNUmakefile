TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(BOOST_LIB_DIR)
#	-L$(TOPDIR)/wtest \

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lboost_filesystem -lboost_system \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o

TEST_OBJS =

TEST_CPP_BINS = \
	testSHA$(EXE) \
	testMD5$(EXE) \
	AES256test$(EXE) \
	CRAMtest$(EXE) \
	HMACtest$(EXE) \
	PBKDF2test$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

testSHA$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
testMD5$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
AES256test$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
CRAMtest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
HMACtest$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a
PBKDF2test$(EXE): $(TOPDIR)/src/libwolframe/libwolframe.a

local_all:

local_clean:
	-rm -f $(TEST_CPP_BINS)
	-rm -f *.o

local_distclean:

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe"

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testSHA
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testMD5
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./CRAMtest
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./AES256test
	@echo
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./HMACtest
	@echo
	@echo
endif

local_longtest: PBKDF2test
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./PBKDF2test
	@echo
	@echo
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
