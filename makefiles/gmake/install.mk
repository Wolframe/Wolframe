# install
#
# requires:
# - DESTDIR: temporary or final destination dir for installation
# - prefix:  prefix for the directory root (e.g. /usr)
#
# provides:
# - target: install
# - target: uninstall

# default values
DESTDIR=
prefix=/usr

# standard directories following FHS
execdir=$(DESTDIR)$(prefix)
bindir=$(execdir)/bin
sbindir=$(execdir)/sbin
libdir=$(execdir)/lib
sysconfdir=$(execdir)/etc
includedir=$(execdir)/include
datadir=$(execdir)/share
localedir=$(datadir)/locale

.PHONY: install_recursive install local_install

install_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d install || exit 1); done)

install: install_recursive local_install install_po
	@test -d "$(libdir)" || mkdir -p "$(libdir)"
	@test -z "$(STATIC_LIB)" || ( \
		$(INSTALL) -m 644 $(STATIC_LIB) $(libdir)/$(STATIC_LIB) )
	@test -z "$(DYNAMIC_LIB)" || ( \
		$(INSTALL) -m 755 $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) \
			$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) )
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" )
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(libdir)/$(DYNAMIC_LIB)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(libdir)/$(DYNAMIC_LIB)" )

.PHONY: uninstall_recursive uninstall local_uninstall

uninstall_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

uninstall: uninstall_recursive local_uninstall uninstall_po
	test -z "$(DYNAMIC_LIB)" || ( \
		test ! -f "$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" || \
			rm "$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" )
	test -z "$(DYNAMIC_LIB)" || ( \
		test ! -h "$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" || \
			rm "$(libdir)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" )
	test -z "$(DYNAMIC_LIB)" || ( \
		test ! -h "$(libdir)/$(DYNAMIC_LIB)" || \
			rm "$(libdir)/$(DYNAMIC_LIB)" )
	test -z "$(STATIC_LIB)" || ( \
		test ! -f "$(libdir)/$(STATIC_LIB)" || rm "$(libdir)/$(STATIC_LIB)" )
