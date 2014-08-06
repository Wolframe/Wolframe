TOPDIR = ..

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

CPP_INCLUDE_SUBDIRS = \
	. \
	langbind cmdbind database database/vm config \
	logger utils serialize serialize/struct \
	serialize/ddl processor filter types AAAA module \
	prgbind crypto system appdevel appdevel/module

C_INCLUDE_SUBDIRS = \
	types crypto

local_install:
	for d in $(CPP_INCLUDE_SUBDIRS); do \
		$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/$$d && \
		$(INSTALL) -m 644 $$d/*.hpp $(DESTDIR)$(includedir)/wolframe/$$d; \
	done
	for d in $(C_INCLUDE_SUBDIRS); do \
		$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/$$d && \
		$(INSTALL) -m 644 $$d/*.h $(DESTDIR)$(includedir)/wolframe/$$d; \
	done

local_uninstall:
	-rm -rf $(DESTDIR)$(includedir)/wolframe
	-rmdir $(DESTDIR)$(includedir).
