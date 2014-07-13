TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

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

# openssl
VARIABLES := $(VARIABLES) WITH_SSL=$(WITH_SSL)

# PAM
VARIABLES := $(VARIABLES) WITH_PAM=$(WITH_PAM)

# Sasl
VARIABLES := $(VARIABLES) WITH_SASL=$(WITH_SASL)

# Sqlite
VARIABLES := $(VARIABLES) WITH_SQLITE3=$(WITH_SQLITE3)

# PostgreSQL
VARIABLES := $(VARIABLES) WITH_PGSQL=$(WITH_PGSQL)

# Oracle
VARIABLES := $(VARIABLES) WITH_ORACLE=$(WITH_ORACLE)

# textwolf
VARIABLES := $(VARIABLES) WITH_TEXTWOLF=$(WITH_TEXTWOLF)

# libxml2 (libxslt)
VARIABLES := $(VARIABLES) WITH_LIBXML2=$(WITH_LIBXML2)

# CJson
VARIABLES := $(VARIABLES) WITH_CJSON=$(WITH_CJSON)

# Lua
VARIABLES := $(VARIABLES) WITH_LUA=$(WITH_LUA)

# Python
VARIABLES := $(VARIABLES) WITH_PYTHON=$(WITH_PYTHON)

# ICU
VARIABLES := $(VARIABLES) WITH_ICU=$(WITH_ICU)

# libharu
VARIABLES := $(VARIABLES) WITH_LIBHPDF=$(WITH_LIBHPDF)

# FreeImage
VARIABLES := $(VARIABLES) WITH_FREEIMAGE=$(WITH_FREEIMAGE)

config-modularized: $(MODULARIZED_CONFIG_FILES)

modularized/wolframe.conf:
	cd templates; VARIANT=modularized \
		$(VARIABLES) \
		../expand_template.sh wolframe-modularized.tmpl \
		../modularized/wolframe.conf

modularized/conf.d/mod_db_sqlite3.conf:
	cd templates; VARIANT=modularized \
		$(VARIABLES) \
		../expand_template.sh sqlite.tmpl \
		../modularized/conf.d/mod_db_sqlite3.conf 
	
modularized/conf.d/mod_db_postgresql.conf:
	cd templates; VARIANT=modularized \
		$(VARIABLES) \
		../expand_template.sh postgresql.tmpl \
		../modularized/conf.d/mod_db_postgresql.conf 

