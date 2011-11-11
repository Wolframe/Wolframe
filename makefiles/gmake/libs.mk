# provides explicit library rules
#
# requires:
# - STATIC_LIB: name of the static library
# - DYNAMIC_LIB: soname and versions of the shared library
# - DYNAMIC_MODULE: loadable module (for dlopen)
#
# - all others like OBJS, CPP_OBJS, LIBS, SH_OBJS, SHPP_OBJS, LDFLAGS
#
# provides:
# - targets to build the static and dynamic version of the project's library
# - targets to build the loadable module
#

ifneq "$(DYNAMIC_LIB)" ""
SONAME=$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifneq "$(DYNAMIC_MODULE)" ""
SONAME=$(DYNAMIC_MODULE)
endif

ifeq "$(PLATFORM)" "LINUX"
SO_FLAGS = -shared -Wl,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(COMPILER)" "gcc"
SO_FLAGS = -shared -Wl,-h,$(SONAME)
endif
ifeq "$(COMPILER)" "spro"
SO_FLAGS = -G -h $(SONAME)
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
SO_FLAGS = -shared -Wl,-x,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "OPENBSD"
SO_FLAGS = -shared -Wl,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "NETBSD"
SO_FLAGS = -shared -Wl,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "CYGWIN"
SO_FLAGS = -shared -Wl,-soname,$(SONAME)
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
$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR) : $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" )
$(DYNAMIC_LIB) : $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(DYNAMIC_LIB)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(DYNAMIC_LIB)" )	
else
$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) :
endif

ifneq "$(DYNAMIC_MODULE)" ""
$(DYNAMIC_MODULE) : $(SH_OBJS) $(SHPP_OBJS)
	$(CCPP_LINK) $(SO_FLAGS) -o $@ $(LDFLAGS) $(SH_OBJS) $(SHPP_OBJS) $(LIBS)
else
$(DYNAMIC_MODULE) :
endif
