TOPDIR = ../../..

#SUBDIRS = database textfile
SUBDIRS =

# PAM
ifeq ($(WITH_PAM),1)
SUBDIRS += pam
endif

# SASL
ifeq ($(WITH_SASL),1)
SUBDIRS += sasl
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_test:
