TOPDIR = ../..

SUBDIRS = audit authentication authorization \
	database datatype filter ddlcompiler \
	normalize prnt cmdbind functions jobSchedule

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe/modules
	$(INSTALL) -d -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe/modules

local_uninstall:
	@-rmdir $(DESTDIR)$(libdir)/wolframe/modules
