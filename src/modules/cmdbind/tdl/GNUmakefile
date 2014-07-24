TOPDIR = ../../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libwolframe_database

INCLUDE_LIBS = \
	-lwolframe \
	-lwolframe_langbind \
	-lwolframe_database

DYNAMIC_MODULE = mod_command_tdl.so


TDL_CPP_OBJS = \
	tdl/auditCallStatement.o \
	tdl/auditElementReference.o \
	tdl/commandDefinition.o \
	tdl/elementReference.o \
	tdl/embeddedStatement.o \
	tdl/parseUtils.o \
	tdl/preprocBlock.o \
	tdl/preprocCallStatement.o \
	tdl/preprocElementReference.o \
	tdl/subroutineCallStatement.o

VM_CPP_OBJS = \
	vm/inputStructure.o \
	vm/program.o \
	vm/selectorPath.o \
	vm/selectorPathSet.o \
	vm/subroutine.o \
	vm/tagTable.o

CPP_OBJS = \
	$(TDL_CPP_OBJS) \
	$(VM_CPP_OBJS) \
	tdlTransactionFunction.o \
	tdlTransactionInput.o \
	tdlTransactionPreprocStep.o \
	loadTransactionProgram.o \
	tdl2vmTranslator.o \
	transactionProgram.o \
	mod_command_tdl.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:

