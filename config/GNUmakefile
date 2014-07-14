TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

local_all: config

.PHONY: config config-monolythic config-modularized

config: config-monolythic config-modularized

VARIABLES=

# openssl
VARIABLES += WITH_SSL=$(WITH_SSL)

# PAM
VARIABLES += WITH_PAM=$(WITH_PAM)

# Sasl
VARIABLES += WITH_SASL=$(WITH_SASL)

# Sqlite
VARIABLES += WITH_SQLITE3=$(WITH_SQLITE3)

# PostgreSQL
VARIABLES += WITH_PGSQL=$(WITH_PGSQL)

# Oracle
VARIABLES += WITH_ORACLE=$(WITH_ORACLE)

# textwolf
VARIABLES += WITH_TEXTWOLF=$(WITH_TEXTWOLF)

# libxml2 (libxslt)
VARIABLES += WITH_LIBXML2=$(WITH_LIBXML2)

# CJson
VARIABLES += WITH_CJSON=$(WITH_CJSON)

# Lua
VARIABLES += WITH_LUA=$(WITH_LUA)

# Python
VARIABLES += WITH_PYTHON=$(WITH_PYTHON)

# ICU
VARIABLES += WITH_ICU=$(WITH_ICU)

# libharu
VARIABLES += WITH_LIBHPDF=$(WITH_LIBHPDF)

# FreeImage
VARIABLES += WITH_FREEIMAGE=$(WITH_FREEIMAGE)

MODULARIZED_CONFIG_FILES = \
	modularized/wolframe.conf \
	modularized/conf.d/mod_db_sqlite3.conf \
	modularized/conf.d/mod_db_postgresql.conf \
	modularized/conf.d/mod_db_oracle.conf

MONOLYTHIC_CONFIG_FILES = \
	monolythic/wolframe.conf
	
.PHONY: $(MODULARIZED_CONFIG_FILES) $(MONOLYTHIC_CONFIG_FILES)

config-modularized: $(MODULARIZED_CONFIG_FILES)

config-monolythic:
	cd templates; VARIANT=monolythic \
		$(VARIABLES) \
		../expand_template.sh wolframe-monolythic.tmpl \
		../monolythic/wolframe.conf

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

modularized/conf.d/mod_db_oracle.conf:
	cd templates; VARIANT=modularized \
		$(VARIABLES) \
		../expand_template.sh oracle.tmpl \
		../modularized/conf.d/mod_db_oracle.conf 
