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
	i18n/form1.de_CH.qm \
	i18n/form1.ro_RO.qm \
	i18n/form2.de_CH.qm \
	i18n/form2.ro_RO.qm \
	i18n/form3.de_CH.qm \
	i18n/form3.ro_RO.qm \
	i18n/form4.de_CH.qm \
	i18n/form4.ro_RO.qm \
	i18n/form5.de_CH.qm \
	i18n/form5.ro_RO.qm \
	i18n/form5_create.de_CH.qm \
	i18n/form5_create.ro_RO.qm \
	i18n/form5_update.de_CH.qm \
	i18n/form5_update.ro_RO.qm \
	i18n/form5_show.de_CH.qm \
	i18n/form5_show.ro_RO.qm

RESOURCES = \
	resources/testcases.rcc

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
	$(QT_LUPDATE) forms/form1.ui -ts i18n/form1.de_CH.ts
	$(QT_LUPDATE) forms/form1.ui -ts i18n/form1.ro_RO.ts
	$(QT_LUPDATE) forms/form2.ui -ts i18n/form2.de_CH.ts
	$(QT_LUPDATE) forms/form2.ui -ts i18n/form2.ro_RO.ts
	$(QT_LUPDATE) forms/form3.ui -ts i18n/form3.de_CH.ts
	$(QT_LUPDATE) forms/form3.ui -ts i18n/form3.ro_RO.ts
	$(QT_LUPDATE) forms/form4.ui -ts i18n/form4.de_CH.ts
	$(QT_LUPDATE) forms/form4.ui -ts i18n/form4.ro_RO.ts
	$(QT_LUPDATE) forms/form5.ui -ts i18n/form5.de_CH.ts
	$(QT_LUPDATE) forms/form5.ui -ts i18n/form5.ro_RO.ts
	$(QT_LUPDATE) forms/form5_create.ui -ts i18n/form5_create.de_CH.ts
	$(QT_LUPDATE) forms/form5_create.ui -ts i18n/form5_create.ro_RO.ts
	$(QT_LUPDATE) forms/form5_update.ui -ts i18n/form5_update.de_CH.ts
	$(QT_LUPDATE) forms/form5_update.ui -ts i18n/form5_update.ro_RO.ts
	$(QT_LUPDATE) forms/form5_show.ui -ts i18n/form5_show.de_CH.ts
	$(QT_LUPDATE) forms/form5_show.ui -ts i18n/form5_show.ro_RO.ts
