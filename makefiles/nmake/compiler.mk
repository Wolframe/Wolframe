# sets compiler settings
#
# requires:
# - INCLUDE_DIRS: directories searched for includes (/I)
# - INCLUDE_CFLAGS: specific compilation flags (C)
# - INCLUDE_CXXFLAGS: specific compilation flags (C++)
# - INCLUDE_LDFLAGS: library flags like like link location (/L)
# - INCLUDE_LIBS: additional libraries to link against (e.g. advapi32.dll)
# provides:
# - generic implicit rules for compilation/linking
#

# TODO: which flags to enable?
# /nologo: disable MS disclaimer
# /EHsc: enable C++ exception handling
# /Ox: optimize what you can
# /Zi: enable debug information
# /MD: multithreaded runtime
# /W <n>: show warnings (level 1 to 4)
# /Wp64: warn about possible 64-bit issues
# using /W2 for now, /W3 shows lots of problems
# in boost/asio/openssl (size_t -> int conversion)
# /Wp64 breaks Qt and SSL
# /Wall: enable all warnings (produces tons of warnings!)
# /WX: treat warnings as errors

# compilation flags and compilers (release)
COMMON_COMPILE_FLAGS = /MD /W2 /nologo /O2 /EHsc /c $(INCLUDE_DIRS)

# compilation flags and compilers (debug)
#COMMON_COMPILE_FLAGS = /MDd /Zi /W2 /WX /nologo /O2 /EHsc /c $(INCLUDE_DIRS)

COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS)

CXX_COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS) /EHsc

CFLAGS = $(COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_CFLAGS) $(DEBUGLEVELFLAGS)
CXXFLAGS = $(CXX_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_CXXFLAGS) $(DEBUGLEVELFLAGS)
CC = cl.exe
CXX = cl.exe
MC = mc.exe
MT = mt.exe
RC = rc.exe

# linking flags (release)
LDFLAGS = /nologo $(INCLUDE_LDFLAGS)

# linking flags (debug)
#LDFLAGS = /nologo /debug /verbose:lib $(INCLUDE_LDFLAGS)

LIBS = $(INCLUDE_LIBS)
LINK = link.exe
CXX_LINK = link.exe

.SUFFIXES: .c .cpp .obj .exe .mc .rc .res

.c.obj:
	$(CC) $(CFLAGS) /Fo$@ $<

.cpp.obj:
	$(CXX) $(CXXFLAGS) /Fo$@ $<

.c.dllobj:
	$(CC) $(CFLAGS) /D "BUILD_SHARED" /Fo$@ $<

.cpp.dllobj:
	$(CXX) $(CXXFLAGS) /D "BUILD_SHARED" /Fo$@ $<

.obj.exe:
	$(CXX_LINK) $(LDFLAGS) $(LIBS) /out:$@ $(OBJS) $**
	$(MT) -nologo -manifest $@.manifest -outputresource:$@;1

.mc.rc:
	"$(MC)" -h $(@D) -r $(@D) $<

.rc.res:
	$(RC) $<
