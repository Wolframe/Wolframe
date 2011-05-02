TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: gtest-all.o

gtest-all.o: src/gtest-all.cc
	g++ -o gtest-all.o -Iinclude -I. -g -Wall -Wextra -c src/gtest-all.cc

local_clean:
	@-rm gtest-all.o

local_distclean:

local_test:

