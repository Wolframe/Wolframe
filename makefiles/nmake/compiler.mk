# sets compiler settings
#
# requires:
# - INCLUDE_DIRS: directories searched for includes (/I)
# - INCLUDE_CFLAGS: specific compilation flags (C)
# - INCLUDE_CPPFLAGS: specific compilation flags (C++)
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
COMMON_COMPILE_FLAGS = /MD /W2 /WX /nologo /O2 /EHsc /c $(INCLUDE_DIRS)

# compilation flags and compilers (debug)
#COMMON_COMPILE_FLAGS = /MDd /Zi /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_SECURE_NO_WARNINGS=1 /W2 /WX /nologo /O2 /EHsc /c $(INCLUDE_DIRS)

COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS)

CCPP_COMPILE_FLAGS = $(COMMON_COMPILE_FLAGS) /EHsc

CFLAGS = $(COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_CFLAGS) $(DEBUGLEVELFLAGS)
CCPPFLAGS = $(CCPP_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_CPPFLAGS) $(DEBUGLEVELFLAGS)
CC = cl.exe
CCPP = cl.exe
MC = mc.exe
MT = mt.exe
RC = rc.exe

# linking flags (release)
LDFLAGS = /nologo $(INCLUDE_LDFLAGS)

# linking flags (debug)
#LDFLAGS = /nologo /debug /verbose:lib $(INCLUDE_LDFLAGS)

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
	$(CCPP_LINK) $(LDFLAGS) $(LIBS) /out:$@ $(OBJS) $**
	$(MT) -nologo -manifest $@.manifest -outputresource:$@;1

.mc.rc:
	"$(MC)" -h $(@D) -r $(@D) $<

.rc.res:
	$(RC) $<
