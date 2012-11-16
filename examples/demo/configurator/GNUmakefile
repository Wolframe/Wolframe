TOPDIR = ../../..

SUBDIRS = 

-include $(TOPDIR)/makefiles/gmake/sub.mk

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_functions:$(TOPDIR)/src/libwolframe_prnt"

ifeq ($(WITH_LIBPQ),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(LIBPQ_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML_LIB_DIR)"
endif

ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

run:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(TOPDIR)/src/wolframed -c wolframe-aba.conf -f

check:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(TOPDIR)/src/wolframed -c wolframe-aba.conf -t

print:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(TOPDIR)/src/wolframed -c wolframe-aba.conf -p
