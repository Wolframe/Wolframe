TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

IMAGES = \
	overview.svg \
	procProvider.svg \
	processing.svg \
	processing2.svg

# Generate graphic files from diagrams
.SUFFIXES: .svg .dia

.dia.svg:
	dia --export=$@ --filter=svg $<


local_all: $(IMAGES)

local_clean:

local_distclean:

local_test:
