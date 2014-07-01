# makefile for a sub package
#
# requires:
# - TOPDIR
# - SUBDIRS
# - INCLUDE_DIRS
#
# provides:
# - target: all
# - target 'test'
# - target 'longtest'
# indirectly (via clean.mk):
# - target 'clean'
# - target 'distclean'

!INCLUDE $(TOPDIR)\makefiles\nmake\platform.mk
!INCLUDE $(TOPDIR)\makefiles\nmake\compiler.mk

all: local_all
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'all' & cd %%d & $(MAKE) /nologo /f Makefile.w32 all & cd ..

test: $(OBJS) $(TEST_OBJS) $(CPPOBJS) $(BIN_OBJS) $(BINS) $(CPP_BINS) $(TEST_BIN_OBJS) $(TEST_BINS) $(TEST_CPP_BINS) local_test
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'test' & cd %%d & $(MAKE) /nologo /f Makefile.w32 test & cd ..

longtest: $(OBJS) $(TEST_OBJS) $(CPPOBJS) $(BIN_OBJS) $(BINS) $(CPP_BINS) $(TEST_BIN_OBJS) $(TEST_BINS) $(TEST_CPP_BINS) local_longtest
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'longtest' & cd %%d & $(MAKE) /nologo /f Makefile.w32 longtest & cd ..

!INCLUDE $(TOPDIR)\makefiles\nmake\clean.mk
