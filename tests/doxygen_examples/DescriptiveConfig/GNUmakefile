TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_database \
	-lwolframe_langbind \
	-lwolframe_serialize \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_program_options \
	$(LIBS_LT) $(LIBS_NET)

CPP_OBJS = \
	src.o

TEST_CPP_BINS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

src$(EXE): $(TOPDIR)/src/libwolframed.a $(TOPDIR)/src/libwolframe/libwolframe.a $(TOPDIR)/src/libwolframe_database/libwolframe_database.a $(TOPDIR)/src/libwolframe_langbind/libwolframe_langbind.a $(TOPDIR)/src/libwolframe_serialize/libwolframe_serialize.a

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_serialize"

local_all:$(CPP_OBJS)

local_clean:
	@rm -f $(CPP_OBJS)

local_distclean:

local_install:

local_uninstall:

local_test:


