TOPDIR = ..

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

INCLUDE_SUBDIRS = . config AAAA processor logger serialize database protocol

local_install:
	for d in $(INCLUDE_SUBDIRS); do \
		$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/$$d && \
		$(INSTALL) -m 644 $$d/*.hpp $(DESTDIR)$(includedir)/wolframe/$$d; \
	done

local_uninstall:
	-rm -rf $(DESTDIR)$(includedir)/wolframe
	-rmdir $(DESTDIR)$(includedir)