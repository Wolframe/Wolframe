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
	i18n/categories.de_CH.qm \
	i18n/categories_features_add.de_CH.qm \
	i18n/categories_features_add.ro_RO.qm \
	i18n/categories_features.de_CH.qm \
	i18n/categories_features_edit.de_CH.qm \
	i18n/categories_features_edit.ro_RO.qm \
	i18n/categories_features.ro_RO.qm \
	i18n/categories.ro_RO.qm \
	i18n/category_add.de_CH.qm \
	i18n/category_add.ro_RO.qm \
	i18n/category_edit.de_CH.qm \
	i18n/category_edit.ro_RO.qm \
	i18n/component_add.de_CH.qm \
	i18n/component_add.ro_RO.qm \
	i18n/component_edit.de_CH.qm \
	i18n/component_edit.ro_RO.qm \
	i18n/components.de_CH.qm \
	i18n/components.ro_RO.qm \
	i18n/feature_add.de_CH.qm \
	i18n/feature_add.ro_RO.qm \
	i18n/feature_edit.de_CH.qm \
	i18n/feature_edit.ro_RO.qm \
	i18n/features.de_CH.qm \
	i18n/features.ro_RO.qm \
	i18n/init.de_CH.qm \
	i18n/init.ro_RO.qm \
	i18n/manufacturer_add.de_CH.qm \
	i18n/manufacturer_add.ro_RO.qm \
	i18n/manufacturer_edit.de_CH.qm \
	i18n/manufacturer_edit.ro_RO.qm \
	i18n/manufacturers.de_CH.qm \
	i18n/manufacturers.ro_RO.qm \
	i18n/picture_add.de_CH.qm \
	i18n/picture_add.ro_RO.qm \
	i18n/picture_edit.de_CH.qm \
	i18n/picture_edit.ro_RO.qm \
	i18n/picture_info.de_CH.qm \
	i18n/picture_info.ro_RO.qm \
	i18n/pictures.de_CH.qm \
	i18n/pictures.ro_RO.qm \
	i18n/tag_add.de_CH.qm \
	i18n/tag_add.ro_RO.qm \
	i18n/tag_edit.de_CH.qm \
	i18n/tag_edit.ro_RO.qm \
	i18n/tags.de_CH.qm \
	i18n/tags.ro_RO.qm

RESOURCES = \
	resources/configurator.rcc

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
	$(QT_LUPDATE) forms/categories_features_add.ui -ts i18n/categories_features_add.de_CH.ts
	$(QT_LUPDATE) forms/categories_features_add.ui -ts i18n/categories_features_add.ro_RO.ts
	$(QT_LUPDATE) forms/categories_features_edit.ui -ts i18n/categories_features_edit.de_CH.ts
	$(QT_LUPDATE) forms/categories_features_edit.ui -ts i18n/categories_features_edit.ro_RO.ts
	$(QT_LUPDATE) forms/categories_features.ui -ts i18n/categories_features.de_CH.ts
	$(QT_LUPDATE) forms/categories_features.ui -ts i18n/categories_features.ro_RO.ts
	$(QT_LUPDATE) forms/categories.ui -ts i18n/categories.de_CH.ts
	$(QT_LUPDATE) forms/categories.ui -ts i18n/categories.ro_RO.ts
	$(QT_LUPDATE) forms/category_add.ui -ts i18n/category_add.de_CH.ts
	$(QT_LUPDATE) forms/category_add.ui -ts i18n/category_add.ro_RO.ts
	$(QT_LUPDATE) forms/category_edit.ui -ts i18n/category_edit.de_CH.ts
	$(QT_LUPDATE) forms/category_edit.ui -ts i18n/category_edit.ro_RO.ts
	$(QT_LUPDATE) forms/component_add.ui -ts i18n/component_add.de_CH.ts
	$(QT_LUPDATE) forms/component_add.ui -ts i18n/component_add.ro_RO.ts
	$(QT_LUPDATE) forms/component_edit.ui -ts i18n/component_edit.de_CH.ts
	$(QT_LUPDATE) forms/component_edit.ui -ts i18n/component_edit.ro_RO.ts
	$(QT_LUPDATE) forms/components.ui -ts i18n/components.de_CH.ts
	$(QT_LUPDATE) forms/components.ui -ts i18n/components.ro_RO.ts
	$(QT_LUPDATE) forms/feature_add.ui -ts i18n/feature_add.de_CH.ts
	$(QT_LUPDATE) forms/feature_add.ui -ts i18n/feature_add.ro_RO.ts
	$(QT_LUPDATE) forms/feature_edit.ui -ts i18n/feature_edit.de_CH.ts
	$(QT_LUPDATE) forms/feature_edit.ui -ts i18n/feature_edit.ro_RO.ts
	$(QT_LUPDATE) forms/features.ui -ts i18n/features.de_CH.ts
	$(QT_LUPDATE) forms/features.ui -ts i18n/features.ro_RO.ts
	$(QT_LUPDATE) forms/init.ui -ts i18n/init.de_CH.ts
	$(QT_LUPDATE) forms/init.ui -ts i18n/init.ro_RO.ts
	$(QT_LUPDATE) forms/manufacturer_add.ui -ts i18n/manufacturer_add.de_CH.ts
	$(QT_LUPDATE) forms/manufacturer_add.ui -ts i18n/manufacturer_add.ro_RO.ts
	$(QT_LUPDATE) forms/manufacturer_edit.ui -ts i18n/manufacturer_edit.de_CH.ts
	$(QT_LUPDATE) forms/manufacturer_edit.ui -ts i18n/manufacturer_edit.ro_RO.ts
	$(QT_LUPDATE) forms/manufacturers.ui -ts i18n/manufacturers.de_CH.ts
	$(QT_LUPDATE) forms/manufacturers.ui -ts i18n/manufacturers.ro_RO.ts
	$(QT_LUPDATE) forms/picture_add.ui -ts i18n/picture_add.de_CH.ts
	$(QT_LUPDATE) forms/picture_add.ui -ts i18n/picture_add.ro_RO.ts
	$(QT_LUPDATE) forms/picture_edit.ui -ts i18n/picture_edit.de_CH.ts
	$(QT_LUPDATE) forms/picture_edit.ui -ts i18n/picture_edit.ro_RO.ts
	$(QT_LUPDATE) forms/picture_info.ui -ts i18n/picture_info.de_CH.ts
	$(QT_LUPDATE) forms/picture_info.ui -ts i18n/picture_info.ro_RO.ts
	$(QT_LUPDATE) forms/pictures.ui -ts i18n/pictures.de_CH.ts
	$(QT_LUPDATE) forms/pictures.ui -ts i18n/pictures.ro_RO.ts
	$(QT_LUPDATE) forms/tag_add.ui -ts i18n/tag_add.de_CH.ts
	$(QT_LUPDATE) forms/tag_add.ui -ts i18n/tag_add.ro_RO.ts
	$(QT_LUPDATE) forms/tag_edit.ui -ts i18n/tag_edit.de_CH.ts
	$(QT_LUPDATE) forms/tag_edit.ui -ts i18n/tag_edit.ro_RO.ts
	$(QT_LUPDATE) forms/tags.ui -ts i18n/tags.de_CH.ts
	$(QT_LUPDATE) forms/tags.ui -ts i18n/tags.ro_RO.ts
