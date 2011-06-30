TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CPPFLAGS =

INCLUDE_DIRS = \
	-I. -I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR)

INCLUDE_LIBS = \
	-lwt -lwthttp -lwt \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_signals -lboost_date_time -lboost_regex \
	-lboost_program_options -lboost_filesystem

OBJS = \

CPP_BINS = \
	wtclient$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(bindir)

local_uninstall:
	-rm -f $(bindir)/wtclient$(EXE)
	-rmdir $(bindir)
	
FORMS = \
	forms/form1.ui \
	forms/form2.ui

THEMES = \
	themes/phone/MainWindow.ui \
	themes/windows/MainWindow.ui

local_test:

HOST=localhost
PORT=8081

run: $(CPP_BINS)
	@echo "Testing Wt webserver, connect to http://$(HOST):$(PORT) now.."
	@./wtclient \
		--docroot htdocs \
		--http-address=$(HOST) \
		--http-port=$(PORT)
		-c etc/wt_config.xml
