TOPDIR = ../../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBPNG_INCLUDE_DIRS) \
	$(LIBZ_INCLUDE_DIRS)

INCLUDE_LDFLAGS = \
	$(LIBPNG_LIB_DIRS) \
	$(LIBZ_LIB_DIRS)

INCLUDE_LIBS = \
	$(LIBPNG_LIBS) \
	$(LIBZ_LIBS)

STATIC_LIB = libhpdf.a

DYNAMIC_LIB = libhpdf.so
DYNAMIC_LIB_MAJOR = 2
DYNAMIC_LIB_MINOR = 2
DYNAMIC_LIB_PATCH = 1

OBJS = \
	hpdf_annotation.o \
	hpdf_array.o \
	hpdf_binary.o \
	hpdf_boolean.o \
	hpdf_catalog.o \
	hpdf_destination.o \
	hpdf_dict.o \
	hpdf_doc.o \
	hpdf_doc_png.o \
	hpdf_encoder.o \
	hpdf_encoder_cns.o \
	hpdf_encoder_cnt.o \
	hpdf_encoder_jp.o \
	hpdf_encoder_kr.o \
	hpdf_encrypt.o \
	hpdf_encryptdict.o \
	hpdf_error.o \
	hpdf_ext_gstate.o \
	hpdf_font.o \
	hpdf_font_cid.o \
	hpdf_fontdef_base14.o \
	hpdf_fontdef.o \
	hpdf_fontdef_cid.o \
	hpdf_fontdef_cns.o \
	hpdf_fontdef_cnt.o \
	hpdf_fontdef_jp.o \
	hpdf_fontdef_kr.o \
	hpdf_fontdef_tt.o \
	hpdf_fontdef_type1.o \
	hpdf_font_tt.o \
	hpdf_font_type1.o \
	hpdf_gstate.o \
	hpdf_image.o \
	hpdf_image_png.o \
	hpdf_info.o \
	hpdf_list.o \
	hpdf_mmgr.o \
	hpdf_name.o \
	hpdf_namedict.o \
	hpdf_null.o \
	hpdf_number.o \
	hpdf_objects.o \
	hpdf_outline.o \
	hpdf_page_label.o \
	hpdf_page_operator.o \
	hpdf_pages.o \
	hpdf_pdfa.o \
	hpdf_real.o \
	hpdf_streams.o \
	hpdf_string.o \
	hpdf_u3d.o \
	hpdf_utils.o \
	hpdf_xref.o \
	hpdf_3dmeasure.o \
	hpdf_encoder_utf.o \
	hpdf_exdata.o \
	hpdf_image_ccitt.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

ALL_CFLAGS = $(OPTFLAGS) -Wall -I. -I../include $(INCLUDE_DIRS) $(INCLUDE_CFLAGS)

local_all:

local_clean:

local_distclean:

local_install:

local_uninstall:

local_test:
