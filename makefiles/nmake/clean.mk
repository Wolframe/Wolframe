# cleans up directories
#
# requires:
# - SUBDIRS: for recursive cleaning
# - local_clean, local_distclean targets in local GNUmakefile
# - all artifacts to clean (the rest is cleaned with wildcards):
#   - OBJS, DLL_OBJS
#
# provides:
# - target: clean
# - target: distclean

clean_recursive:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'clean' & cd %%d & $(MAKE) /nologo /f Makefile.w32 clean & cd ..

clean: clean_recursive local_clean
	-@erase *.bak 2>NUL
	-@erase *~ 2>NUL
	-@erase *.exe 2>NUL
	-@erase *.exe.manifest 2>NUL
	-@erase *.obj >NUL 2>NUL
	-@erase *.pdb 2>NUL
	-@erase *.res 2>NUL
	-@erase MSG*.bin 2>NUL
	-@erase *.dllobj 2>NUL
	-@erase *.dll 2>NUL
	-@erase *.lib 2>NUL
	-@erase *.dllobj >NUL 2>NUL
	-@erase *.exp 2>NUL
	-@erase *.ilk 2>NUL
	-@erase *.idb 2>NUL
	-@erase *.manifest 2>NUL

distclean_recursive:
	@if not "$(SUBDIRS)" == "" @for %%d IN ( $(SUBDIRS) ) do @echo Entering directory '%%CD%\%%d' doing 'distclean' & cd %%d & $(MAKE) /nologo /f Makefile.w32 distclean & cd ..

distclean: distclean_recursive local_distclean clean
