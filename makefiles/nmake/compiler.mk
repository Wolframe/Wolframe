# sets compiler settings
#
# requires:
# - INCLUDE_DIRS
#
# provides:
# - BIN_OBJS: the object files we need for the binaries which we build always
# - CPP_BIN_OBJS: same for binaries which have C++ code in them
# - TEST_BIN_OBJS: same as BIN_OBJS but for test binaries compiled only when
#   testing
# - TEST_CPP_BIN_OBJS: same for C++ tests
#

# TODO: which flags to enable?
# /nologo: disable MS disclaimer
# /MT: create a multi-thread binary
# /EHsc: enable C++ exception handling
# /Ox: optimize what you can
# /Zi: enable debug information
# /MTD: create multi-threaded debug binary
# /we<n>: show warnings (level 1 to 4)
# /Wall: enable all warnings

# compilation flags and compilers (release)
COMMON_COMPILE_FLAGS = /MD /nologo /O2 /c $(INCLUDE_DIRS)

# compilation flags and compilers (debug)
#COMMON_COMPILE_FLAGS = /MDd /ZI /nologo /c $(INCLUDE_DIRS) /RTC1

COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS)

CCPP_COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS) /EHsc

CFLAGS = $(COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(DEBUGLEVELFLAGS)
CCPPFLAGS = $(CCPP_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(DEBUGLEVELFLAGS)
CC = cl.exe
CCPP = cl.exe
MC = "$(PLATFORM_SDK_DIR)\Bin\mc.exe"
RC = "$(PLATFORM_SDK_DIR)\Bin\rc.exe"

# linking flags (release)
LDFLAGS = /nologo $(INCLUDE_LDFLAGS)

# linking flags (debug)
#LDFLAGS = /nologo /debug $(INCLUDE_LDFLAGS)

LIBS = $(INCLUDE_LIBS)
LINK = link.exe
CCPP_LINK = link.exe

.SUFFIXES: .c .cpp .obj .exe .mc .rc .res

.c.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

.cpp.obj:
	$(CCPP) $(CCPPFLAGS) /Fo$@ $<

.c.dllobj:
	$(CC) $(CFLAGS) /D "BUILD_SHARED" /Fo$@ $<

.cpp.dllobj:
	$(CCPP) $(CCPPFLAGS) /D "BUILD_SHARED" /Fo$@ $<

.obj.exe:
	$(CCPP_LINK) $(LDFLAGS) $(LIBS) /out:$@ $< $(OBJS)

.mc.rc:
	$(MC) -h $(@D) -r $(@D) $<

.rc.res:
	$(RC) $<

#%$(EXE): %.o $(OBJS) $(TEST_OBJS)
#	$(LINK) -o $@ $(LDFLAGS) $(OBJS) $(TEST_OBJS) $< $(LIBS)

#%.sho : %.c
#	$(CC) -c -o $@ -fPIC -DSHARED $(CFLAGS) $<

#%$(SO) : %.sho $(OBJS)
#	$(LINK) -shared -o $@ $(LDFLAGS) $(LIBS) $(OBJS) $<

#%.sho++ : %.cpp
#	$(CCPP) -c -o $@ -fPIC -DSHARED $(CCPPFLAGS) $<

#%$(SO) : %.sho++ $(OBJS) $(CPPOBJS)
#	$(CCPP_LINK) -shared -o $@ $(LDFLAGS) $(LIBS) $(OBJS) $(CPPOBJS) $<
 
#BIN_OBJS = $(BINS:$(EXE)=.o) 
#TEST_BIN_OBJS = $(TEST_BINS:$(EXE)=.o)
#CPP_BIN_OBJS = $(CPP_BINS:$(EXE)=.o)
#TEST_CPP_BIN_OBJS = $(TEST_CPP_BINS:$(EXE)=.o)
