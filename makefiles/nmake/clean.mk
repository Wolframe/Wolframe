# cleans up directories
#
# requires:
# - SUBDIRS: for recursive cleaning
# - local_clean, local_distclean targets in local GNUmakefile
# - all artifacts to clean:
#   - BINS, TEST_BINS, TEST_CPP_BINS, CPP_BINS
#   - OBJS, CPPOBJS, BIN_OBJS, TEST_BIN_OBJS, CPP_BIN_OBJS, TEST_CPP_BIN_OBJS
#   - CMODULES, CPPMODULES
#
# provides:
# - target: clean
# - target: distclean

clean_recursive:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 clean & cd ..

#clean: clean_recursive local_clean
#	-@rm -f *.d port/*.d 2>/dev/null
#	-@rm -f $(BINS) $(CPP_BINS) $(TEST_BINS) $(TEST_CPP_BINS) 2>/dev/null
#	-@rm -f $(OBJS) $(CPP_OBJS) $(BIN_OBJS) $(TEST_BIN_OBJS) $(CPP_BIN_OBJS) $(TEST_CPP_BIN_OBJS) 2>/dev/null
#	-@rm -f exec/*
#	-@rm -f *.core
#	-@rm -f $(CMODULES) $(CPPMODULES)
#	-@rm -f $(CMODULES .o=.d) $(CPPMODULES .o=.d)
clean: clean_recursive local_clean
	-@erase *.bak 2>NUL
	-@erase *~ 2>NUL
	-@erase *.d 2>NUL
	-@erase *.exe 2>NUL
	-@erase *.exe.manifest 2>NUL
	-@erase *.obj 2>NUL
	-@erase $(OBJS) 2>NUL
	-@erase *.pdb 2>NUL
	-@erase *.rc 2>NUL
	-@erase *.res 2>NUL
	-@erase MSG*.bin 2>NUL
	-@erase *.dllobj 2>NUL
	-@erase $(DLL_OBJS) 2>NUL
	-@erase *.exp 2>NUL
	-@erase *.ilk 2>NUL
	-@erase *.idb 2>NUL
	-@erase *.manifest 2>NUL

distclean_recursive:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @cd %%d & $(MAKE) /nologo /f Makefile.w32 distclean & cd ..

distclean: distclean_recursive local_distclean clean
