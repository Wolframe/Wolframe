# provides explicit library rules
#
# requires:
# - STATIC_LIB: name of the static library
# - DYNAMIC_LIB: soname and versions of the shared library
# - all others like OBJS, CPP_OBJS, LIBS, SH_OBJS, SHPP_OBJS, LDFLAGS
#
# provides:
# - targets to build the static and dynamic version of the project's library
#

ifeq "$(PLATFORM)" "LINUX"
SO_FLAGS = -shared -Wl,-soname,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(COMPILER)" "gcc"
SO_FLAGS = -shared -Wl,-h,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif
ifeq "$(COMPILER)" "spro"
SO_FLAGS = -G -h $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
SO_FLAGS = -shared -Wl,-x,-soname,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifeq "$(PLATFORM)" "OPENBSD"
SO_FLAGS = -shared -Wl,-soname,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifeq "$(PLATFORM)" "NETBSD"
SO_FLAGS = -shared -Wl,-soname,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifeq "$(PLATFORM)" "CYGWIN"
SO_FLAGS = -shared -Wl,-soname,$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifneq "$(STATIC_LIB)" ""
$(STATIC_LIB) : $(OBJS) $(CPP_OBJS)
	$(AR) cr $@ $(OBJS) $(CPP_OBJS)
else
$(STATIC_LIB) :
endif

ifneq "$(DYNAMIC_LIB)" ""
$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) : $(SH_OBJS) $(SHPP_OBJS)
	$(CCPP_LINK) $(SO_FLAGS) -o $@ $(LDFLAGS) $(SH_OBJS) $(SHPP_OBJS) $(LIBS)
else
$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) :
endif
