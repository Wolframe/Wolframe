# default values
DESTDIR=
prefix=/usr

# standard directories following FHS
execdir=$(prefix)
bindir=$(execdir)/bin
sbindir=$(execdir)/sbin
libdir=$(execdir)/$(LIBDIR)
sysconfdir=$(execdir)/etc
includedir=$(execdir)/include
datadir=$(execdir)/share
mandir=$(execdir)/share
localedir=$(datadir)/locale

# Wolframe specific directories
moduleloaddir=$(libdir)/wolframe/modules
