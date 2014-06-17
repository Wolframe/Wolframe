TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-DWITH_PAM

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(PAM_INCLUDE_DIR) \
	-I$(TOPDIR)/src/modules/authentication/pam

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(PAM_LIB_DIR)

INCLUDE_LIBS = \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(TOPDIR)/src/libwolframed.a \
	$(TOPDIR)/src/libwolframe/libwolframe.a \
	$(TOPDIR)/src/libwfsingleton/libwfsingleton.a \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system \
	$(PAM_LIBS)

CPP_OBJS = \
	$(TOPDIR)/src/modules/authentication/pam/PAMAuth.o

TEST_CPP_BINS = \
	pamTest$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/wtest"

LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(PAM_LIB_DIR)"

local_test: $(TEST_CPP_BINS)
ifeq ($(RUN_TESTS),1)
# Aba: running PAM tests needs to much from the system, can't test automatically currently
#ifeq ($(WITH_PAM),1)
#	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./pamTest
#	@echo
#endif
endif

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
