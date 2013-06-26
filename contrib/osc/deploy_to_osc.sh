#!/bin/sh

VERSION=0.0.1
OSC_HOME=$HOME/home:andreas_baumann/Wolframe

# the original package
rm -f wolframe-$VERSION.tar.gz
make \
	WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	dist-gz
cp wolframe-$VERSION.tar.gz $OSC_HOME/wolframe_$VERSION.tar.gz

# the Redhat build script
cp redhat/wolframe.spec $OSC_HOME/wolframe.spec

# patches
cp contrib/osc/boost/boost_1_48_0-gcc-compile.patch $OSC_HOME/.

# compute sizes of packages
SIZE=`stat -c '%s' $OSC_HOME/wolframe_$VERSION.tar.gz`
CHKSUM=`md5sum $OSC_HOME/wolframe_$VERSION.tar.gz | cut -f 1 -d' '`

# copy all Debian versions of the description files.
cp contrib/osc/wolframe*.dsc $OSC_HOME
for i in `ls $OSC_HOME/wolframe*.dsc`; do
	echo " $CHKSUM $SIZE wolframe_$VERSION.tar.gz" >> $i
done

# Archlinux specific files
cat contrib/osc/PKGBUILD > $OSC_HOME/PKGBUILD
cp archlinux/wolframe.conf $OSC_HOME/wolframe.conf
cp archlinux/wolframed.service $OSC_HOME/wolframed.service
cp archlinux/wolframe.install $OSC_HOME/wolframe.install

CHKSUM2=`md5sum $OSC_HOME/wolframe.conf | cut -f 1 -d' '`
CHKSUM3=`md5sum $OSC_HOME/wolframed.service | cut -f 1 -d' '`

echo "md5sums=('$CHKSUM' '$CHKSUM2' '$CHKSUM3')" >> $OSC_HOME/PKGBUILD
