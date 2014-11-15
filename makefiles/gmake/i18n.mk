# handle localization stuff (gettext)
# (this follows roughly the way Postgresql handles NLS)
#
# requires:
# - CATALOG_NAME: name of the catalog (name of the library or program)
#                 will be installed as $(CATALOG_NAME).po in the localedir
# - GETTEXT_LANGUAGES: list of languages supported
# - GETTEXT_FILES: list of source files that contain message strings
# - GETTEXT_TRIGGERS: (optional) list of functions/macros that contain
#                     translatable strings
#
# provides:
# - target: init-po
# - target: merge-po
# - target: check-po
# - target: install_po
# - target: uninstall_po

.PHONY: init-po update-po all_po clean_po install_po uninstall_po

ifeq "$(ENABLE_NLS)" "1"

PO_FILES = $(addprefix po/, $(addsuffix .po, $(GETTEXT_LANGUAGES)))
MO_FILES = $(addprefix po/, $(addsuffix .mo, $(GETTEXT_LANGUAGES)))

%.mo : %.po
	$(MSGFMT) -c -o $@ $<

po/$(CATALOG_NAME).pot: $(GETTEXT_FILES)
	$(XGETTEXT) -n -F -d $(CATALOG_NAME) -o $@ \
		$(addprefix -k, $(GETTEXT_TRIGGERS)) --flag=_:1:pass-c-format \
		-d $(CATALOG_NAME) -n -F $(GETTEXT_FILES)

ifneq "$(GETTEXT_FILES)" ""
init-po: po/$(CATALOG_NAME).pot
else
init-po:
endif

ifneq "$(GETTEXT_FILES)" ""
merge-po: po/$(CATALOG_NAME).pot
	@test -z "$(GETTEXT_LANGUAGES)" || \
		for lang in $(GETTEXT_LANGUAGES)""; do ( \
			echo "merging po/$$lang.mo and $<.." && \
			if $(MSGMERGE) -F -o po/$$lang.po.new po/$$lang.po $<; then \
				cp po/$$lang.po po/$$lang.po.bak && \
				mv po/$$lang.po.new po/$$lang.po; \
			else \
				echo "Please check, msgmerge for po/$$lang.po failed!"; \
				rm -f po/$$lang.po.new; \
			fi \
		) done
else
merge-po:
endif

check-po: $(PO_FILES)
	@test -z "$^" || \
		for file in $^""; do ( \
			echo "checking gettext file $$file of catalog $(CATALOG_NAME).." && \
			$(MSGFMT) -c -v -o /dev/null $$file || exit 1 \
		) done

clean_po:
	@-rm -f $(MO_FILES) 2>/dev/null
	@-rm -f po/$(CATALOG_NAME).pot 2>/dev/null
	@-rm -f po/*.bak 2>/dev/null
	@-rm -f po/*~ 2>/dev/null

# hooks for the standard targets handling gettext stuff

all_po: $(MO_FILES)

install_po:
	@test -z "$(GETTEXT_LANGUAGES)" || \
		for lang in $(GETTEXT_LANGUAGES)""; do ( \
			echo "installing po/$$lang.mo to $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES.." && \
			$(INSTALL) -d -m 755 $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES && \
			$(INSTALL) -m 644 po/$$lang.mo $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(CATALOG_NAME).mo || exit 1 \
		) done

uninstall_po:
	@test -z "$(GETTEXT_LANGUAGES)" || \
		for lang in $(GETTEXT_LANGUAGES)""; do ( \
			echo "uninstalling $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES.." && \
			rm $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(CATALOG_NAME).mo && \
			rmdir $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES || exit 1 \
		) done

else

# No NLS supported wanted, provide dummy targets

init-po:
merge-po:
check-po:

all_po:
clean_po:
install_po:
uninstall_po:

endif
