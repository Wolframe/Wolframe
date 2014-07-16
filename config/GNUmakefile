TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

local_all: config

.PHONY: config config-monolythic config-modularized config-packaging

config: config-monolythic config-modularized config-packaging

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

PACKAGING_CONFIG_FILES = \
	../packaging/debian/wolframe.conf \
	../packaging/redhat/wolframe.conf \
	../packaging/archlinux/wolframe.conf \
	../packaging/slackware/wolframe.conf \
	../packaging/freebsd/wolframe.conf \
	../packaging/netbsd/wolframe.conf \
	../packaging/solaris/wolframe.conf

.PHONY: $(PACKAGING_CONFIG_FILES)

config-packaging: $(PACKAGING_CONFIG_FILES)

# example configuration with typical layouts
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

# Per packaging configuration

../packaging/archlinux/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=arch \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/archlinux/wolframe.conf

../packaging/redhat/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=redhat \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/redhat/wolframe.conf

../packaging/debian/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=debian \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/debian/wolframe.conf

../packaging/slackware/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=slackware \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/slackware/wolframe.conf

../packaging/freebsd/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=FREEBSD \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/freebsd/wolframe.conf

../packaging/netbsd/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=NETBSD \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/netbsd/wolframe.conf

../packaging/solaris/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=SUNOS \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../../packaging/solaris/wolframe.conf
