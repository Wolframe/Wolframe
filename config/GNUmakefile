TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all: config

.PHONY: config config-monolythic config-modularized

config: config-monolythic config-modularized

config-monolythic:
	cd templates; VARIANT=monolythic \
		../expand_template.sh wolframe-monolythic.tmpl \
		../monolythic/wolframe.conf

MODULELARIZED_CONFIG_FILES = \
	modularized/wolframe.conf \
	modularized/conf.d/mod_db_sqlite3.conf
	
config-modularized: $(MODULELARIZED_CONFIG_FILES)

modularized/conf.d/mod_db_sqlite3.conf: templates/sqlite.tmpl
	cd templates; VARIANT=modularized \
		../expand_template.sh sqlite.tmpl \
		../modularized/conf.d/mod_db_sqlite3.conf 
	
modularized/wolframe.conf: templates/wolframe-modularized.tmpl
	cd templates; VARIANT=modularized \
		../expand_template.sh wolframe-modularized.tmpl \
		../modularized/wolframe.conf
