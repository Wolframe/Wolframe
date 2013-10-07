TOPDIR = ../../..

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

DIST_HEADERS = \
	hpdf_3dmeasure.h \
	hpdf_annotation.h \
	hpdf_catalog.h \
	hpdf_conf.h \
	hpdf_config.h \
	hpdf_consts.h \
	hpdf_destination.h \
	hpdf_doc.h \
	hpdf_encoder.h \
	hpdf_encryptdict.h \
	hpdf_encrypt.h \
	hpdf_error.h \
	hpdf_exdata.h \
	hpdf_ext_gstate.h \
	hpdf_fontdef.h \
	hpdf_font.h \
	hpdf_gstate.h \
	hpdf.h \
	hpdf_image.h \
	hpdf_info.h \
	hpdf_list.h \
	hpdf_mmgr.h \
	hpdf_namedict.h \
	hpdf_objects.h \
	hpdf_outline.h \
	hpdf_page_label.h \
	hpdf_pages.h \
	hpdf_pdfa.h \
	hpdf_streams.h \
	hpdf_types.h \
	hpdf_u3d.h \
	hpdf_utils.h \
	hpdf_version.h

local_install:
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/wolframe/libhpdf
	$(INSTALL) -m 644 $(DIST_HEADERS) $(DESTDIR)$(includedir)/wolframe/libhpdf

local_uninstall:
	-rm -rf $(DESTDIR)$(includedir)/wolframe/libhpdf/*
	-rmdir $(DESTDIR)$(includedir)/wolframe/libhpdf
	-rmdir $(DESTDIR)$(includedir)/wolframe

local_test:
