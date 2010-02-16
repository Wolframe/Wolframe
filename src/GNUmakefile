TOPDIR = ..

SUBDIRS =

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I.

INCLUDE_LIBS = \

CPP_BINS = \
	bunnyd$(EXE)

STATIC_LIBS = \

STATIC_LIB_OBJS = \

C_PORT_OBJS = \

OBJS = \
	$(C_PORT_OBJS) \
	$(STATIC_LIB_OBJS) \
	connection.o \
	mime_types.o \
	posix_main.o \
	reply.o \
	request_handler.o \
	request_parser.o \
	server.o

bunnyd$(EXE):	$(OBJS)
	g++ -o bunnyd$(EXE) $(OBJS) -lboost_thread -lboost_system

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
