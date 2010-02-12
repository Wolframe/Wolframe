# provides generic rules for C/C++ dependeny generation using
# 'makedepend', 'gcc -MM' or similar mechanisms
#
# requires:
# - compilers CC and CCPP
# - INCLUDEDIRS
# - OBJS, CPP_OBJS and BIN_OBJS, CPP_BIN_OBJS
# - TEST_BINS, TEST_BIN_OBJS, TEST_CPP_BINS, TEST_CPP_BIN_OBJS
#
# provides:
# - included dependency files
#
# author: Andreas Baumann, abaumann at yahoo dot com

ifeq "$(COMPILER)" "gcc"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(CFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CCPP) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(CCPPFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

endif

ifeq "$(COMPILER)" "tcc"

%.d : %.c
	@echo Generating dependencies for $<
	@makedepend -DMAKE_DEPENDENCIES $(PLATFORM_COMPILE_FLAGS) $(INCLUDE_DIRS) -I/usr/lib/tcc/include -f - $< > $@

endif

ifeq "$(COMPILER)" "icc"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT  $(@:.d=.o) $(CFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CCPP) -DMAKE_DEPENDENCIES -MM -MT  $(@:.d=.o) $(CCPPFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

endif

ifeq "$(COMPILER)" "spro"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -xM1 $(CFLAGS) $< > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CCPP) -DMAKE_DEPENDENCIES -xM1 $(CCPPFLAGS) $< > $@
endif

ifeq "$(COMPILER)" "pcc"

# FIXME: platform in path of compiler include files, mmh, how to fix?
%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES $(CFLAGS) -M $< > $@

endif

-include $(OBJS:.o=.d)
-include $(CPP_OBJS:.o=.d)
-include $(BIN_OBJS:.o=.d)
-include $(CPP_BIN_OBJS:.o=.d)
-include $(TEST_BIN_OBJS:.o=.d)
-include $(TEST_CPP_BIN_OBJS:.o=.d)
