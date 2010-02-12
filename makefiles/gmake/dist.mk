# creates a source tarball ready for distribution
#
# requires:
# - PACKAGE_NAME
# - PACKAGE_VERSION
#
# provides:
# - target: dist
# - target: dist-bz2, dist-gz, dist-Z

.PHONY: dist dist-bz2 dist-gz dist-Z

dist:
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)
	-@rm -rf /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	mkdir /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	cp -r * /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION)/.
	-@cd /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION) ; \
		$(MAKE) distclean ; \
		rm -f makefiles/gmake/platform.mk.vars; \
		find . -name .svn -exec rm -rf {} \; ; \
		find . -name .git -exec rm -rf {} \; ; \
		cd .. ; \
		tar cvf $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar \
			$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	-@rm -rf /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	@mv /tmp/$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar .

dist-bz2: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).bz2
	@bzip2 -f $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

dist-gz: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).gz
	@gzip $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

dist-Z: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).Z
	@compress -f $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

