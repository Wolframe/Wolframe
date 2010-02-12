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

.PHONY: uninstall_recursive uninstall local_uninstall

uninstall_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

uninstall: uninstall_recursive local_uninstall uninstall_po
