# install
#
# requires:
# - DESTDIR: temporary or final destination dir for installation
# - prefix:  prefix for the directory root (e.g. /usr)
#
# provides:
# - target: install
# - target: uninstall

.PHONY: install_recursive install local_install

install_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d install || exit 1); done)

install: install_recursive local_install install_po
	@test -d "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)" || mkdir -p "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)"
	@test -z "$(STATIC_LIB)" || echo "$(INSTALL) -m 644 $(STATIC_LIB) $(DESTDIR)$(libdir)/$(PACKAGE_NAME)/"
	@test -z "$(STATIC_LIB)" || ( \
		$(INSTALL) -m 644 $(STATIC_LIB) $(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(STATIC_LIB) )
	@test -z "$(DYNAMIC_LIB)" || echo "$(INSTALL) -m 755 $(DYNAMIC_LIB) $(DESTDIR)$(libdir)/$(PACKAGE_NAME)/"
	@test -z "$(DYNAMIC_LIB)" || ( \
		$(INSTALL) -m 755 $(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) \
			$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH) )
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" )
	@test -z "$(DYNAMIC_LIB)" || ( \
		rm -f "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB)" && \
		ln -s "$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" \
			"$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB)" )

.PHONY: uninstall_recursive uninstall local_uninstall

uninstall_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

uninstall: uninstall_recursive local_uninstall uninstall_po
	@test -z "$(DYNAMIC_LIB)" || echo "rm $(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB)*"
	@test -z "$(DYNAMIC_LIB)" || ( \
		test ! -f "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" || \
			rm "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR).$(DYNAMIC_LIB_MINOR).$(DYNAMIC_LIB_PATCH)" )
	@test -z "$(DYNAMIC_LIB)" || ( \
		test ! -h "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" || \
			rm "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB).$(DYNAMIC_LIB_MAJOR)" )
	@test -z "$(DYNAMIC_LIB)" || ( \
		test ! -h "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB)" || \
			rm "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(DYNAMIC_LIB)" )
	@test -z "$(STATIC_LIB)" || echo "rm $(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(STATIC_LIB)"
	@test -z "$(STATIC_LIB)" || ( \
		test ! -f "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(STATIC_LIB)" || rm "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/$(STATIC_LIB)" )
