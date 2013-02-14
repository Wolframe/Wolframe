TOPDIR = ../../../..

SUBDIRS =

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:
	
%.qm : %.ts
	$(QT_LRELEASE) $< -qm $@

%.rcc : %.qrc
	$(QT_RCC) -binary $< -o $@
	
I18NS = \
	i18n/init.de_CH.qm \
	i18n/init.ro_RO.qm \
	i18n/activities.de_CH.qm \
	i18n/activities.ro_RO.qm \
	i18n/activity_add.de_CH.qm \
	i18n/activity_add.ro_RO.qm \
	i18n/activity_edit.de_CH.qm \
	i18n/activity_edit.ro_RO.qm \
	i18n/customers.de_CH.qm \
	i18n/customers.ro_RO.qm \
	i18n/customer_add.de_CH.qm \
	i18n/customer_add.ro_RO.qm \
	i18n/customer_edit.de_CH.qm \
	i18n/customer_edit.ro_RO.qm \
	i18n/notes.de_CH.qm \
	i18n/notes.ro_RO.qm \
	i18n/note_add.de_CH.qm \
	i18n/note_add.ro_RO.qm \
	i18n/note_edit.de_CH.qm \
	i18n/note_edit.ro_RO.qm

RESOURCES = \
	resources/fulltutorial.rcc

I18NS_TS = $(I18NS:.qm=.ts)

local_all: $(I18NS) $(RESOURCES)

local_clean:
	@-rm -f $(I18NS)
	@-rm -f $(RESOURCES)

local_distclean:

local_install:

local_uninstall:

local_test:

.PHONY: buildi18n
buildi18n:
	$(QT_LUPDATE) forms/init.ui -ts i18n/init.de_CH.ts
	$(QT_LUPDATE) forms/init.ui -ts i18n/init.ro_RO.ts
	$(QT_LUPDATE) forms/activities.ui -ts i18n/activities.de_CH.ts
	$(QT_LUPDATE) forms/activities.ui -ts i18n/activities.ro_RO.ts
	$(QT_LUPDATE) forms/activity_add.ui -ts i18n/activity_add.de_CH.ts
	$(QT_LUPDATE) forms/activity_add.ui -ts i18n/activity_add.ro_RO.ts
	$(QT_LUPDATE) forms/activity_edit.ui -ts i18n/activity_edit.de_CH.ts
	$(QT_LUPDATE) forms/activity_edit.ui -ts i18n/activity_edit.ro_RO.ts
	$(QT_LUPDATE) forms/customers.ui -ts i18n/customers.de_CH.ts
	$(QT_LUPDATE) forms/customers.ui -ts i18n/customers.ro_RO.ts
	$(QT_LUPDATE) forms/customer_add.ui -ts i18n/customer_add.de_CH.ts
	$(QT_LUPDATE) forms/customer_add.ui -ts i18n/customer_add.ro_RO.ts
	$(QT_LUPDATE) forms/customer_edit.ui -ts i18n/customer_edit.de_CH.ts
	$(QT_LUPDATE) forms/customer_edit.ui -ts i18n/customer_edit.ro_RO.ts
	$(QT_LUPDATE) forms/notes.ui -ts i18n/notes.de_CH.ts
	$(QT_LUPDATE) forms/notes.ui -ts i18n/notes.ro_RO.ts
	$(QT_LUPDATE) forms/note_add.ui -ts i18n/note_add.de_CH.ts
	$(QT_LUPDATE) forms/note_add.ui -ts i18n/note_add.ro_RO.ts
	$(QT_LUPDATE) forms/note_edit.ui -ts i18n/note_edit.de_CH.ts
	$(QT_LUPDATE) forms/note_edit.ui -ts i18n/note_edit.ro_RO.ts
