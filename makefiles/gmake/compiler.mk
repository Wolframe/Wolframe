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

# start of gcc section

ifeq "$(COMPILER)" "gcc"

GCC_MAJOR_VERSION ?=	$(shell $(TOPDIR)/makefiles/gmake/guess_env --gcc-major-version $(CC) "$(CURDIR)" $(TOPDIR))
GCC_MINOR_VERSION ?=	$(shell $(TOPDIR)/makefiles/gmake/guess_env --gcc-minor-version $(CC) "$(CURDIR)" $(TOPDIR))

# -Werror: troubles Qt in qvector2d.h
# -Wswitch-default: not good for switches with enums
# -Wsystem-headers: bad idea, as header files are usually happily broken :-)
# -Wtraditional: we don't want to program tradition K&R C anymore!
# -Wunsafe-loop-optimizations: ??
# -Wno-attributes, -Wmissing-format-attribute: ?? later
# -Wpacked -Wpadded: ?? very questionable
# -Wunreachable-code: doesn't work
# -Wno-div-by-zero: we get NaN and friend over macros, so need for funny tricks :-)
# -Wstrict-overflow=5 is relatively new, later maybe
# -fstack-protector or -fstack-protector-all: should be used, but U
#  have currently big problems to get it around compiler gcc and -lssl
#  probing! FIXME later
# -fstack-protector-all: does something funny to the shared objects..
# -Wstack-protector makes no sense without SSP
# everything implied by -Wall is not explicitly specified (gcc 4.2.3)
# -Waggregate-return: is for K&R code and mostly useless nowadays
# -Wno-long-long: some boost code header require 'long long'
# -Winline: warns too often is early optimization anyway, not very useful
# -Wundef: gcc warning is violating the standard, not using it
# -Wvariadic-macros: set -Wno-variadic-macros; troubles gtest, which is happilly
#  mixing C99 and C++98 features, let's hope c++0 sorts this out

# compilation flags and compilers
COMMON_COMPILE_FLAGS = \
	-O2 -g -pipe \
	-fstrict-aliasing \
	-pedantic -Wall \
	-Wno-long-long \
	-Wunused -Wno-import \
	-Wformat -Wformat-y2k -Wformat-nonliteral -Wformat-security -Wformat-y2k \
	-Wswitch-enum -Wunknown-pragmas -Wfloat-equal \
	-Wshadow -Wpointer-arith \
	-Wcast-qual -Wcast-align \
	-Wwrite-strings \
	-Wmissing-noreturn \
	-Wno-multichar -Wparentheses -Wredundant-decls \
	-Wdisabled-optimization
ifeq "$(GCC_MAJOR_VERSION)" "4"
COMMON_COMPILE_FLAGS += \
	-Wfatal-errors -Wmissing-include-dirs \
	-Wvolatile-register-var \
	-Wextra -Winit-self \
	-Wno-variadic-macros
# -Wconversion had to meanings before gcc 4.3 (warn about ABI changes when porting
# old K&R code without function prototypes) and warn about conversions loosing
# precision. So we enable only -Wconversion (not -Wtraditional-conversion) for gcc
# >= 4.3 and no -Wconversion for older gcc versions!
# (see also http://gcc.gnu.org/wiki/NewWconversion)
#ifeq "$(GCC_MINOR_VERSION)" "4"
#COMMON_COMPILE_FLAGS += -Wconversion
#endif

endif

ifeq "$(GCC_MAJOR_VERSION)" "3"

# gcc 3.3, testend on OpenBSD 4.2
ifeq "$(GCC_MINOR_VERSION)" "3"
COMMON_COMPILE_FLAGS += \
	-W
endif

# gcc 3.4, not tested yet
ifeq "$(GCC_MINOR_VERSION)" "4"
COMMON_COMPILE_FLAGS += \
	-Wextra -Winit-self
endif

endif

STD99_COMPILE_FLAGS = \
	-std=c99

COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	$(STD99_COMPILE_FLAGS) \
	-Wnonnull \
	-Wbad-function-cast -Wstrict-prototypes \
	-Wmissing-prototypes -Wmissing-declarations \
	-Wnested-externs

# gcc 4.x
ifeq "$(GCC_MAJOR_VERSION)" "4"
COMPILE_FLAGS += \
	-Wc++-compat -Wdeclaration-after-statement -Wold-style-definition \
	-funit-at-a-time
endif

ifeq "$(GCC_MAJOR_VERSION)" "3"

# gcc 3.4, not tested yet
ifeq "$(GCC_MINOR_VERSION)" "4"
COMPILE_FLAGS += \
	-Wdeclaration-after-statement -Wold-style-definition \
	-funit-at-a-time
endif

# gcc 3.3, testend on OpenBSD 4.2
ifeq "$(GCC_MINOR_VERSION)" "3"
#COMPILE_FLAGS += \
#	-Wdeclaration-after-statement
endif

endif

CXX_COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	-std=c++98

# gcc 4.x
ifeq "$(GCC_MAJOR_VERSION)" "4"
CXX_COMPILE_FLAGS += \
	-Wno-invalid-offsetof -funit-at-a-time
endif

ifeq "$(GCC_MAJOR_VERSION)" "3"

# gcc 3.4, not tested yet
ifeq "$(GCC_MINOR_VERSION)" "4"
CXX_COMPILE_FLAGS += \
	-Wno-invalid-offsetof
endif

# gcc 3.3, testend on OpenBSD 4.2
ifeq "$(GCC_MINOR_VERSION)" "3"
#CXX_COMPILE_FLAGS += \
#	-Wdeclaration-after-statement
endif

endif

#CC = gcc
CXX = g++

endif

# end of gcc section

# start of icc section

# currently we don't need this, the icc flags are fairly consistent
#ICC_MAJOR_VERSION ?=	$(shell $(TOPDIR)/makefiles/gmake/guess_env --icc-major-version $(CC) "$(CURDIR)" $(TOPDIR))
#ICC_MINOR_VERSION ?=	$(shell $(TOPDIR)/makefiles/gmake/guess_env --icc-minor-version $(CC) "$(CURDIR)" $(TOPDIR))

# -vec-report0: turn of SSE2 vector usage messages (they are common since P-4 anyway!)

ifeq "$(COMPILER)" "icc"
COMPILE_FLAGS = \
	-Wall -Werror -w1 -vec-report0
endif

# end of icc section

# start of spro section

# -xc99=all: full C99 compliance for the code (syntax and library functions)
# -Xc: full ISO compliance, no K&R stuf
# -mt: enable mutlithreading (-D_REENTRANT for header files, -lthread for ld)
# -errwarn=%all: convert all warnings to errors
# -v: do more restrictive syntax checking

ifeq "$(COMPILER)" "spro"
STD99_COMPILE_FLAGS = \
	-xc99=all
COMPILE_FLAGS = \
	$(STD99_COMPILE_FLAGS) -Xc -errwarn=%all -mt -v
endif

# end of spro section

ifeq "$(COMPILER)" "icc"
ifeq "$(PLATFORM)" "LINUX"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif
endif

# set flags for position independend code (as required for shared libraries
# on some platforms)
ifeq "$(COMPILER)" "gcc"
ifeq "$(PLATFORM)" "LINUX"
SO_COMPILE_FLAGS = -fPIC
endif
ifeq "$(PLATFORM)" "SUNOS"
SO_COMPILE_FLAGS = -fPIC
endif
ifeq "$(PLATFORM)" "FREEBSD"
SO_COMPILE_FLAGS = -fPIC
endif
ifeq "$(PLATFORM)" "NETBSD"
SO_COMPILE_FLAGS = -fPIC
endif
ifeq "$(PLATFORM)" "OPENBSD"
SO_COMPILE_FLAGS = -fPIC
endif
ifeq "$(PLATFORM)" "CYGWIN"
# code on Cygwin is always position independend
SO_COMPILE_FLAGS =
endif
endif

ifeq "$(COMPILER)" "icc"
ifeq "$(PLATFORM)" "LINUX"
SO_COMPILE_FLAGS = -fPIC
endif
endif

# TODO: test this
#ifeq "$(COMPILER)" "spro"
#ifeq "$(PLATFORM)" "SUNOS"
#ifeq "$(ARCH)" "sun4u"
#SO_COMPILE_FLAGS = -xcode=pic32
#endif
#endif
#endif

ALL_CFLAGS = $(CFLAGS) $(COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_DIRS) $(INCLUDE_CFLAGS) $(PTHREADS_CFLAGS)
ALL_CXXFLAGS = $(CXXFLAGS) $(CXX_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_DIRS) $(INCLUDE_CXXFLAGS) $(PTHREADS_CFLAGS)
ALL_LDFLAGS = $(LDFLAGS) $(INCLUDE_LDFLAGS) $(PTHREADS_LDFLAGS) $(LDFLAGS_NET) $(LDFLAGS_LT)

LIBS = $(INCLUDE_LIBS) $(PTHREADS_LIBS) $(LIBS_NET) $(LIBS_LT)
LINK = $(CC)
CXX_LINK = $(CXX)

%.o : %.c
	$(CC) -c -o $@ $(ALL_CFLAGS) $<

%.o : %.cpp
	$(CXX) -c -o $@ $(ALL_CXXFLAGS) $<

%$(EXE): %.o $(OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $< $(LIBS)

%.sho : %.c
	$(CC) -c -o $@ $(SO_COMPILE_FLAGS) -DSHARED $(ALL_CFLAGS) $<

#%$(SO) : %.sho $(OBJS)
#	$(LINK) -shared -o $@ $(ALL_LDFLAGS) $(LIBS) $(OBJS) $<

%.sho++ : %.cpp
	$(CXX) -c -o $@ $(SO_COMPILE_FLAGS) -DSHARED $(ALL_CXXFLAGS) $<

#%$(SO) : %.sho++ $(OBJS) $(CPPOBJS)
#	$(CXX_LINK) -shared -o $@ $(ALL_LDFLAGS) $(LIBS) $(OBJS) $(CPPOBJS) $<

BIN_OBJS = $(BINS:$(EXE)=.o)
TEST_BIN_OBJS = $(TEST_BINS:$(EXE)=.o)
CPP_BIN_OBJS = $(CPP_BINS:$(EXE)=.o)
TEST_CPP_BIN_OBJS = $(TEST_CPP_BINS:$(EXE)=.o)
SH_OBJS = $(OBJS:.o=.sho)
SHPP_OBJS = $(CPP_OBJS:.o=.sho++)
