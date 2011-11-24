# provides generic rules for C/C++ dependeny generation using
# 'makedepend', 'gcc -MM' or similar mechanisms
#
# requires:
# - compilers CC and CXX
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
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CXX) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CXXFLAGS) $< | \
		sed "s,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g" | tr -s '&' "\n" > $@

endif

ifeq "$(COMPILER)" "spro"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -xM1 $(ALL_CFLAGS) $< > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CXX) -DMAKE_DEPENDENCIES -xM1 $(ALL_CXXFLAGS) $< > $@
endif

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(OBJS:.o=.d)
-include $(CPP_OBJS:.o=.d)
-include $(BIN_OBJS:.o=.d)
-include $(CPP_BIN_OBJS:.o=.d)
-include $(TEST_BIN_OBJS:.o=.d)
-include $(TEST_CPP_BIN_OBJS:.o=.d)

.PHONY: depend
depend: $(OBJS:.o=.d) $(CPP_OBJS:.o=.d) $(BIN_OBJS:.o=.d) $(CPP_BIN_OBJS:.o=.d) $(TEST_BIN_OBJS:.o=.d) $(TEST_CPP_BIN_OBJS:.o=.d)

endif
endif
