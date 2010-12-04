TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS = \

INCLUDE_DIRS = \
	-I$(TOPDIR)/include -I. \
	-I$(TOPDIR)/src \
	-I$(BOOST_DIR)/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_DIR)/lib

INCLUDE_LIBS = \
	-lboost_unit_test_framework \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_filesystem -lboost_system -lboost_thread$(BOOST_LIBRARY_TAG) \
	$(GTEST_LIBS)

TEST_OBJS = \
	$(TOPDIR)/src/version.o \
	$(TOPDIR)/src/logger.o

TEST_CPP_BINS = \
	versionTest$(EXE) \
	counterTest$(EXE) \
	threadLeakTest$(EXE) \
	pechoHandlerTest$(EXE) \
	logTest$(EXE)

ifdef WITH_GTEST
TEST_CPP_BINS += \
	version-gTest$(EXE) \
	counter-gTest$(EXE) \
	pechoHandler-gTest$(EXE)
endif

include $(TOPDIR)/makefiles/gmake/sub.mk

pechoHandlerTest$(EXE): pechoHandlerTest.cpp $(TOPDIR)/src/logger.cpp $(TOPDIR)/src/pechoHandler.cpp $(TOPDIR)/src/pechoHandler.hpp $(TOPDIR)/src/protocol.hpp testHandlerTemplates.hpp
	$(CCPP) -c -o pechoHandlerTest.o $(CCPPFLAGS) pechoHandlerTest.cpp
	$(CCPP) -c -o logger.o $(CCPPFLAGS) $(TOPDIR)/src/logger.cpp
	$(CCPP) -c -o pechoHandler.o $(CCPPFLAGS) $(TOPDIR)/src/pechoHandler.cpp
	$(CCPP_LINK) -o pechoHandlerTest$(EXE) logger.o pechoHandler.o pechoHandlerTest.o

ifdef WITH_GTEST
pechoHandler-gTest$(EXE): pechoHandler-gTest.cpp $(TOPDIR)/src/logger.cpp $(TOPDIR)/src/pechoHandler.cpp $(TOPDIR)/src/pechoHandler.hpp $(TOPDIR)/src/protocol.hpp testHandlerTemplates.hpp
	$(CCPP) -c -o pechoHandler-gTest.o $(CCPPFLAGS) $(INCLUDE_DIRS) pechoHandler-gTest.cpp
	$(CCPP) -c -o logger.o $(CCPPFLAGS) $(TOPDIR)/src/logger.cpp
	$(CCPP) -c -o pechoHandler.o $(CCPPFLAGS) $(TOPDIR)/src/pechoHandler.cpp
	$(CCPP_LINK) -o pechoHandler-gTest$(EXE) $(INCLUDE_LDFLAGS) $(GTEST_LIBS) logger.o pechoHandler.o pechoHandler-gTest.o
endif

xmlselectHandlerTest$(EXE): xmlselectHandlerTest.cpp $(TOPDIR)/src/logger.cpp $(TOPDIR)/src/xmlselectHandler.cpp $(TOPDIR)/src/xmlselectHandler.hpp $(TOPDIR)/src/protocol.hpp testHandlerTemplates.hpp
	$(CCPP) -c -o xmlselectHandlerTest.o $(CCPPFLAGS) xmlselectHandlerTest.cpp
	$(CCPP) -c -o logger.o $(CCPPFLAGS) $(TOPDIR)/src/logger.cpp
	$(CCPP) -c -o xmlselectHandler.o $(CCPPFLAGS) $(TOPDIR)/src/xmlselectHandler.cpp
	$(CCPP_LINK) -o xmlselectHandlerTest$(EXE) logger.o xmlselectHandler.o xmlselectHandlerTest.o

local_all:

local_clean:
	-rm -rf logTest.log

local_distclean:

local_test:
	@echo
	@echo "Testing version..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./versionTest
	@echo "Testing counter.."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./counterTest
ifdef WITH_GTEST
	@echo "Testing with Google test framework..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./version-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./counter-gTest
	@echo
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./pechoHandler-gTest
endif
	@echo
	@echo "Testing boost log..."
	@LD_LIBRARY_PATH=$(BOOST_DIR)/lib ./logTest
	@echo
