# provides explicit library rules
#
# requires:
# - STATIC_LIB: name of the static library
# - DYNAMIC_LIB: soname and versions of the shared library
# - DYNAMIC_MODULE: loadable module (for dlopen)
#
# - all others like OBJS, CPP_OBJS, LIBS, LDFLAGS
#
# provides:
# - targets to build the static and dynamic version of the project's library
# - targets to build the loadable module (no soname and funny installation
#   rules here)
#

# the soname of the shared library
ifneq "$(DYNAMIC_LIB)" ""
SONAME=$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)
endif

ifneq "$(NOINST_DYNAMIC_LIB)" ""
SONAME=$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR)
endif

# SONAME_FLAGS to indicate to the platform linker, we want to fiddle in the
# ELF header (more plaform/linker dependant than compiler dependant)

ifeq "$(PLATFORM)" "LINUX"
SONAME_FLAGS=-Wl,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(COMPILER)" "gcc"  
SONAME_FLAGS=-Wl,-h,$(SONAME)
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
SONAME_FLAGS=-Wl,-x,-soname,$(SONAME)
endif

ifeq "$(PLATFORM)" "NETBSD"
SONAME_FLAGS=-Wl,-x,-soname,$(SONAME)
endif

# no soname and versioning for loadable modules, just dynamic linking
ifneq "$(DYNAMIC_MODULE)" ""
SONAME_FLAGS=
endif

# indicate we want to link shared, depends actually on the compiler more
# than on the platform
ifeq "$(COMPILER)" "gcc"
SO_LIB_FLAGS = -shared $(SONAME_FLAGS)
SO_MOD_FLAGS = -shared
endif

ifeq "$(COMPILER)" "clang"
SO_LIB_FLAGS = -shared $(SONAME_FLAGS)
SO_MOD_FLAGS = -shared
endif

ifeq "$(COMPILER)" "icc"
SO_LIB_FLAGS = -shared $(SONAME_FLAGS)
SO_MOD_FLAGS = -shared
endif

# build rule for a static library

ifneq "$(STATIC_LIB)" ""
$(STATIC_LIB) : $(OBJS) $(CPP_OBJS)
	$(AR) cr $@ $(OBJS) $(CPP_OBJS)
else
$(STATIC_LIB) :
endif

ifneq "$(NOINST_STATIC_LIB)" ""
$(NOINST_STATIC_LIB) : $(OBJS) $(CPP_OBJS)
	$(AR) cr $@ $(OBJS) $(CPP_OBJS)
else
$(NOINST_STATIC_LIB) :
endif

# build rule for a dynamic library

ifneq "$(DYNAMIC_LIB)" ""
$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) : $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) $(SO_LIB_FLAGS) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $(LIBS)
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

ifneq "$(NOINST_DYNAMIC_LIB)" ""
$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH) : $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) $(SO_LIB_FLAGS) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $(LIBS)
$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR) : $(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH)
	@test -z "$(NOINST_DYNAMIC_LIB)" || ( \
		rm -f "$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR)" && \
		ln -s "$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH)" \
			"$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR)" )
$(NOINST_DYNAMIC_LIB) : $(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH)
	@test -z "$(NOINST_DYNAMIC_LIB)" || ( \
		rm -f "$(NOINST_DYNAMIC_LIB)" && \
		ln -s "$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH)" \
			"$(NOINST_DYNAMIC_LIB)" )	
else
$(NOINST_DYNAMIC_LIB).$(NOINST_DYNAMIC_LIB_MAJOR).$(NOINST_DYNAMIC_LIB_MINOR).$(NOINST_DYNAMIC_LIB_PATCH) :
endif

# build rule for a dynamic module, no soname is added to the library
ifneq "$(DYNAMIC_MODULE)" ""
$(DYNAMIC_MODULE) : $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) $(SO_MOD_FLAGS) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $(LIBS)
else
$(DYNAMIC_MODULE) :
endif
