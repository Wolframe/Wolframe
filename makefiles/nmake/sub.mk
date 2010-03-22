# makefile for a sub package
#
# requires:
# - TOPDIR
# - SUBDIRS
# - INCLUDE_DIRS
#
# provides:
# - target: all targets

!include $(TOPDIR)\makefiles\nmake\platform.mk
!include $(TOPDIR)\makefiles\nmake\compiler.mk

all: local_all
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 all & cd ..

#.PHONY: all $(SUBDIRS) local_all
#all: $(OBJS) $(CPPOBJS) $(BIN_OBJS) $(CPP_BIN_OBJS) $(BINS) $(CPP_BINS) $(CMODULES) $(CPPMODULES) local_all
#	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
#	  (set -e; $(MAKE) -C $$d all || exit 1); done)

test: $(OBJS) $(TEST_OBJS) $(CPPOBJS) $(BIN_OBJS) $(BINS) $(CPP_BINS) $(TEST_BIN_OBJS) $(TEST_BINS) $(TEST_CPP_BINS) $(CMODULES) $(CPPMODULES) local_test
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 test & cd ..

#-include $(TOPDIR)/makefiles/gmake/depend.mk
!include $(TOPDIR)\makefiles\nmake\clean.mk
#-include $(TOPDIR)/makefiles/gmake/install.mk
