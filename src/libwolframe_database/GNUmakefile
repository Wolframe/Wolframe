TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem

STATIC_LIB = libwolframe_database.a

DYNAMIC_LIB = libwolframe_database.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

CPP_OBJS = \
	transaction.o \
	transactionInput.o \
	transactionOutput.o \
	transactionFunction.o \
	transactionFunctionDescription.o \
	transactionfunction/TagTable.o \
	transactionfunction/Path.o \
	transactionfunction/PreProcessCommand.o \
	transactionfunction/DatabaseCommand.o \
	transactionfunction/InputStructure.o \
	transactionfunction/ResultStructure.o \
	transactionfunction/ResultIterator.o \
	transactionfunction/ResultFilter.o \
	loadTransactionProgram.o \
	databaseLanguage.o \
	transactionExecStatemachine.o \
	bindStatementParams.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

