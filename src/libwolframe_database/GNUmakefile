TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe


INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LIBS = \
	-lwfsingleton -lwolframe

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
	baseStatement.o \
	substitutingStatement.o \
	tdl/elementReference.o \
	tdl/embeddedStatement.o \
	tdl/parseUtils.o \
	tdl/preprocCallStatement.o \
	tdl/preprocElementReference.o \
	tdl/preprocBlock.o \
	tdl/subroutineCallStatement.o \
	vm/selectorPathSet.o \
	vm/subroutine.o \
	vm/program.o \
	vm/programInstance.o \
	vm/tdlTranslatorInterface.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

