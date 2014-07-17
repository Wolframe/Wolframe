TOPDIR = ..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:
	
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
	redhat/wolframe.conf \
	debian/wolframe.conf \
	archlinux/wolframe.conf \
	slackware/wolframe.conf \
	freebsd/wolframe.conf \
	netbsd/wolframe.conf \
	solaris/wolframe.conf

.PHONY: $(PACKAGING_CONFIG_FILES)

config-packaging: $(PACKAGING_CONFIG_FILES)

.PHONY: redhat debian archlinux slackware freebsd netbsd solaris

redhat: redhat/wolframe.conf
debian: debian/wolframe.conf
archlinux: archlinux/wolframe.conf
slackware: slackware/wolframe.conf
freebsd: freebsd/wolframe.conf
netbsd: netbsd/wolframe.conf
solaris: solaris/wolframe.conf

REDHAT_VARIABLES = \
	WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
	WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
	WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1

DEBIAN_VARIABLES = \
	WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
	WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
	WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1

# example configurations with typical layouts, later also more
# complex AAAA scenarios or contributed configurations

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

# per packaging configuration

redhat/wolframe.conf:
	cd templates; VARIANT=modularized \
		../expand_template.sh wolframe-modularized.tmpl \
		../redhat/wolframe.conf
	cp modularized/conf.d/* redhat/conf.d/.
	cp modularized/modules.d/* redhat/modules.d/.
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=redhat \
		$(REDHAT_VARIABLES) \
		../expand_template.sh sqlite.tmpl \
		../redhat/conf.d/mod_db_sqlite3.conf 	
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=redhat \
		$(REDHAT_VARIABLES) \
		../expand_template.sh postgresql.tmpl \
		../redhat/conf.d/mod_db_postgresql.conf 
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=redhat \
		$(REDHAT_VARIABLES) \
		../expand_template.sh oracle.tmpl \
		../redhat/conf.d/mod_db_oracle.conf 

debian/wolframe.conf:
	cd templates; VARIANT=modularized \
		../expand_template.sh wolframe-modularized.tmpl \
		../debian/wolframe.conf
	cp modularized/conf.d/* debian/conf.d/.
	cp modularized/modules.d/* debian/modules.d/.
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=debian \
		$(REDHAT_VARIABLES) \
		../expand_template.sh sqlite.tmpl \
		../debian/conf.d/mod_db_sqlite3.conf 	
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=debian \
		$(REDHAT_VARIABLES) \
		../expand_template.sh postgresql.tmpl \
		../debian/conf.d/mod_db_postgresql.conf 
	cd templates; VARIANT=modularized \
		PLATFORM=LINUX LINUX_DIST=debian \
		$(REDHAT_VARIABLES) \
		../expand_template.sh oracle.tmpl \
		../debian/conf.d/mod_db_oracle.conf 

archlinux/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=arch \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../archlinux/wolframe.conf

slackware/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=LINUX LINUX_DIST=slackware \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../slackware/wolframe.conf

freebsd/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=FREEBSD \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../freebsd/wolframe.conf

netbsd/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=NETBSD \
		WITH_SSL=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../netbsd/wolframe.conf

solaris/wolframe.conf:
	cd templates; VARIANT=monolythic \
		PLATFORM=SUNOS \
		WITH_SSL=1 WITH_PAM=1 WITH_SASL=1 WITH_SQLITE3=1 WITH_PGSQL=1 \
		WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_ICU=1 \
		WITH_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
		../expand_template.sh wolframe-monolythic.tmpl \
		../solaris/wolframe.conf

# layout to install per platform

ifeq "$(PLATFORM)" "LINUX"
ifeq "$(LINUX_DIST)" "redhat"
local_install: install_common install_modularized
endif
ifeq "$(LINUX_DIST)" "sles"
local_install: install_common install_modularized
endif
ifeq "$(LINUX_DIST)" "suse"
local_install: install_common install_modularized
endif
ifeq "$(LINUX_DIST)" "debian"
local_install: install_common install_modularized
endif
ifeq "$(LINUX_DIST)" "ubuntu"
local_install: install_common install_modularized
endif
ifeq "$(LINUX_DIST)" "arch"
local_install: install_common
endif
ifeq "$(LINUX_DIST)" "slackware"
local_install: install_common
endif
endif
ifeq "$(PLATFORM)" "FREEBSD"
local_install: install_common
endif
ifeq "$(PLATFORM)" "NETBSD"
local_install: install_common
endif
ifeq "$(PLATFORM)" "SUNOS"
local_install: install_common
endif

# install only configuration files for things which are actually built

ifeq ($(WITH_PAM),1)  
MODCONF += $(LINUX_DIST)/conf.d/mod_auth_pam.conf
LOADCONF += $(LINUX_DIST)/modules.d/mod_auth_pam.conf
endif

ifeq ($(WITH_SASL),1)
MODCONF += $(LINUX_DIST)/conf.d/mod_auth_sasl.conf
LOADCONF += $(LINUX_DIST)/modules.d/mod_auth_sasl.conf
endif

ifeq ($(WITH_SQLITE3),1)
MODCONF += $(LINUX_DIST)/conf.d/mod_db_sqlite3.conf
LOADCONF += $(LINUX_DIST)/modules.d/mod_db_sqlite3.conf
endif

ifeq ($(WITH_PGSQL),1)
MODCONF += $(LINUX_DIST)/conf.d/mod_db_postgresql.conf
LOADCONF += $(LINUX_DIST)/modules.d/mod_db_postgresql.conf
endif

ifeq ($(WITH_ORACLE),1)
MODCONF += $(LINUX_DIST)/conf.d/mod_db_oracle.conf
LOADCONF += $(LINUX_DIST)/modules.d/mod_db_oracle.conf
endif

ifeq ($(WITH_TEXTWOLF),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_filter_textwolf.conf
endif

ifeq ($(WITH_LIBXML2),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_filter_libxml2.conf
endif

ifeq ($(WITH_CJSON),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_filter_cjson.conf
endif

ifeq ($(WITH_LUA),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_command_lua.conf
endif

ifeq ($(WITH_PYTHON),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_command_python.conf
endif

ifeq ($(WITH_LIBHPDF),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_print_harupdf.conf
endif

ifeq ($(WITH_FREEIMAGE),1)
LOADCONF += $(LINUX_DIST)/modules.d/mod_graphix.conf
endif

.PHONY: install_common install_modularized

install_common:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sysconfdir)/wolframe
	$(INSTALL) -m 0664 $(LINUX_DIST)/wolframe.conf $(DESTDIR)$(sysconfdir)/wolframe

install_modularized:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sysconfdir)/wolframe/conf.d
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sysconfdir)/wolframe/modules.d
	$(INSTALL) -m 0644 $(MODCONF) $(DESTDIR)$(sysconfdir)/wolframe/conf.d/.
	$(INSTALL) -m 0644 $(LOADCONF) $(DESTDIR)$(sysconfdir)/wolframe/modules.d/.

local_uninstall:
	-rm -rf $(DESTDIR)$(sysconfdir)/wolframe
	-rmdir $(DESTDIR)$(sysconfdir)

