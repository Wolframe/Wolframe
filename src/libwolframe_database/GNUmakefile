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

TDL_CPP_OBJS = \
	tdl/elementReference.o \
	tdl/embeddedStatement.o \
	tdl/parseUtils.o \
	tdl/preprocCallStatement.o \
	tdl/preprocElementReference.o \
	tdl/preprocBlock.o \
	tdl/subroutineCallStatement.o \
	tdl/commandDefinition.o

VM_CPP_OBJS = \
	vm/tagTable.o \
	vm/inputStructure.o \
	vm/selectorPath.o \
	vm/selectorPathSet.o \
	vm/subroutine.o \
	vm/program.o \
	vm/programInstance.o

CPP_OBJS = \
	$(TDL_CPP_OBJS) \
	$(VM_CPP_OBJS) \
	transaction.o \
	tdlTransactionPreprocStep.o \
	tdlTransactionFunction.o \
	vmTransactionInput.o \
	vmTransactionOutput.o \
	tdl2vmTranslator.o \
	loadTransactionProgram2.o \
	databaseLanguage.o \
	baseStatement.o \
	substitutingStatement.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:

