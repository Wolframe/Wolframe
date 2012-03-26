TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS = \
	$(LIBPNG_INCLUDE_DIRS)

INCLUDE_LDFLAGS =

INCLUDE_LIBS =

LOCAL_STATIC_LIB_OBJS = \
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
	hpdf_image_ccitt.o \
	hpdf_pdfa.o

LOCAL_STATIC_LIB = \
	libhpdf.a

-include $(TOPDIR)/makefiles/gmake/sub.mk

# overwrite flags so they fit to how Lua people compile the software.
# we get too many spurios errors and problems (for instance in abuild
# checks on openSuSE). This doesn't mean they should not be fixed
# though upstream!
CFLAGS = -O2 -Wall -I. -I../include

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)
	@rm -f *.sho++

local_distclean:

local_install:

local_test:
