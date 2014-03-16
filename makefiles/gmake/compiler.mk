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

# optimization flags
ifdef RELEASE
OPTFLAGS ?= -O2
# no -frepo -fno-implicit-templates currently, create tons of errors!!
else
OPTFLAGS ?= -g -O0
endif

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
# -Wmissing-return: functions returning and throwing exceptions are no longer
#  possible, this is an annoying feature!
# -Wfloat-equal: Qt header files get more and more broken with this

# compilation flags and compilers
COMMON_COMPILE_FLAGS = \
	$(OPTFLAGS) \
	-fstrict-aliasing -Wstrict-aliasing=2 \
	-pedantic -Wall \
	-Wno-long-long \
	-Wunused -Wno-import \
	-Wformat -Wformat-y2k -Wformat-nonliteral -Wformat-security \
	-Wswitch-enum -Wunknown-pragmas \
	-Wshadow -Wpointer-arith \
	-Wcast-qual -Wcast-align \
	-Wwrite-strings \
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

CC ?= gcc
CXX ?= g++

endif

# end of gcc section

# start of clang section

ifeq "$(COMPILER)" "clang"
COMMON_COMPILE_FLAGS = \
	-Wall -Wextra

CXX_COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	-std=c++98

COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	-std=c99
endif

# end of clang section

# start of icc section

ifeq "$(COMPILER)" "icc"
COMMON_COMPILE_FLAGS = \
	-Wall

CXX_COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	-std=c++98

COMPILE_FLAGS = \
	$(COMMON_COMPILE_FLAGS) \
	-std=c99
endif

# end of icc section

# set flags for threading support using POSIX threads. This is completly different
# between compiler/platforms
ifeq "$(COMPILER)" "gcc"
ifeq "$(PLATFORM)" "LINUX"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif
ifeq "$(PLATFORM)" "SUNOS"
PTHREADS_CFLAGS = -D_REENTRANT -pthreads
PTHREADS_LDFLAGS = -pthreads
PTHREADS_LIBS = 
endif
ifeq "$(PLATFORM)" "FREEBSD"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif
ifeq "$(PLATFORM)" "NETBSD"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif

endif

ifeq "$(COMPILER)" "clang"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif

ifeq "$(COMPILER)" "icc"
PTHREADS_CFLAGS = -D_REENTRANT -pthread
PTHREADS_LDFLAGS = -pthread
PTHREADS_LIBS =
endif

SO_COMPILE_FLAGS = -fPIC

ALL_CFLAGS = $(CFLAGS) $(COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_DIRS) $(INCLUDE_CFLAGS) $(PTHREADS_CFLAGS) $(SO_COMPILE_FLAGS)
ALL_CXXFLAGS = $(CXXFLAGS) $(CXX_COMPILE_FLAGS) $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_DIRS) $(INCLUDE_CXXFLAGS) $(PTHREADS_CFLAGS) $(SO_COMPILE_FLAGS)
ALL_LDFLAGS = $(LDFLAGS) $(INCLUDE_LDFLAGS) $(LDFLAGS_NET) $(LDFLAGS_LT) $(LDFLAGS_DL) $(PTHREADS_LDFLAGS)

LIBS = $(INCLUDE_LIBS) $(PTHREADS_LIBS) $(LIBS_NET) $(LIBS_LT) $(LIBS_DL)
LINK = $(CC)
CXX_LINK = $(CXX)

%.o : %.c
	$(CC) -c -o $@ $(ALL_CFLAGS) $<

%.o : %.cpp
	$(CXX) -c -o $@ $(ALL_CXXFLAGS) $<

%$(EXE): %.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

BIN_OBJS = $(BINS:$(EXE)=.o)
TEST_BIN_OBJS = $(TEST_BINS:$(EXE)=.o)
CPP_BIN_OBJS = $(CPP_BINS:$(EXE)=.o)
TEST_CPP_BIN_OBJS = $(TEST_CPP_BINS:$(EXE)=.o)
