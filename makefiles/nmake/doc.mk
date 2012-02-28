# makefile for a sub package
#
# requires:
# - TOPDIR
#
# provides:
# - target: help

!INCLUDE $(TOPDIR)\makefiles\nmake\platform.mk

all: local_all

test: local_test

clean: local_clean

help:
	@more $(TOPDIR)\makefiles\nmake\dochelp.mk
