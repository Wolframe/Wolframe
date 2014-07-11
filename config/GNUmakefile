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

MODULARIZED_CONFIG_FILES = \
	modularized/wolframe.conf \
	modularized/conf.d/mod_db_sqlite3.conf \
	modularized/conf.d/mod_db_postgresql.conf

.PHONY: $(MODULARIZED_CONFIG_FILES)
	
config-modularized: $(MODULARIZED_CONFIG_FILES)

modularized/wolframe.conf:
	cd templates; VARIANT=modularized \
		../expand_template.sh wolframe-modularized.tmpl \
		../modularized/wolframe.conf

modularized/conf.d/mod_db_sqlite3.conf:
	cd templates; VARIANT=modularized \
		../expand_template.sh sqlite.tmpl \
		../modularized/conf.d/mod_db_sqlite3.conf 
	
modularized/conf.d/mod_db_postgresql.conf:
	cd templates; VARIANT=modularized \
		../expand_template.sh postgresql.tmpl \
		../modularized/conf.d/mod_db_postgresql.conf 
