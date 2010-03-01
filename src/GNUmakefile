TOPDIR = ..

SUBDIRS =

INCLUDE_DIRS = -I. \
	-I$(TOPDIR)/include

INCLUDE_LIBS = \

CPP_BINS = \
	smerpd$(EXE)

STATIC_LIBS = \

STATIC_LIB_OBJS = \

C_PORT_OBJS = \

OBJS = \
	$(C_PORT_OBJS) \
	$(STATIC_LIB_OBJS) \
	posixMain.o \
	reply.o \
	server.o\
	connection.o \
	requestHandler.o


smerpd$(EXE):	$(OBJS)
	g++ -o smerpd$(EXE) $(OBJS) -lboost_thread -lboost_system

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: $(CPP_BINS)

$(STATIC_LIBS): $(STATIC_LIBS_OBJS)
	ar rcs $(STATIC_LIBS) $(STATIC_LIB_OBJS)


local_clean:
	@rm -f $(STATIC_LIBS)

local_distclean:

local_install:
	$(INSTALL) -d -m 755 $(bindir)
	$(INSTALL) -m 644 $(BINS) $(bindir)
