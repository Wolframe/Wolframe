TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/src/modules/authentication/textfile

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwfsingleton/libwfsingleton.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system

CPP_OBJS = \
	$(TOPDIR)/src/modules/authentication/textfile/TextFileAuth.o \
	$(TOPDIR)/src/modules/authentication/textfile/passwdFile.o

TEST_CPP_BINS = \
	passwdHashTest$(EXE) \
	passwdFileTest$(EXE) \
	textFileTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

authenticationTest$(EXE): $(CPP_OBJS)
passwdHashTest$(EXE): $(CPP_OBJS)
passwdFileTest$(EXE): $(CPP_OBJS)
textFileTest$(EXE): $(CPP_OBJS)

local_all:

local_clean:

local_distclean:

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./passwdHashTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./passwdFileTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe ./textFileTest
	@echo
endif
