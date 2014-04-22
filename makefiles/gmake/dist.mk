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

TMPDIR ?= /tmp

dist:
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)
	-@rm -rf $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	@mkdir $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	@cp -r * $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)/.
	-@cd $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION) ; \
		$(MAKE) distclean ; \
		rm -f makefiles/gmake/platform.mk.vars; \
		rm -f makefiles/gmake/platform.vars; \
		find . -name .git -exec rm -rf {} \; ; \
		cd .. ; \
		tar cvf $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar \
			$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	-@rm -rf $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)
	@mv $(TMPDIR)/$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar .

dist-xz: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).xz
	@xz -f $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

dist-bz2: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).bz2
	@bzip2 -f $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

dist-gz: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).gz
	@gzip $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

dist-Z: dist
	-@rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION).Z
	@compress -f $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar

