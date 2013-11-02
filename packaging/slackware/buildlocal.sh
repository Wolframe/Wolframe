#!/bin/sh

VERSION=0.0.1
PKGBUILD=$HOME/slackbuild
ARCH=`uname -m`
if test "x$ARCH" = "xx86_64"; then
	LIBDIR="/usr/lib64"
else
if test "x$ARCH" = "xi686"; then
	LIBDIR="/usr/lib"
else
	echo "ERROR: Unknown slackware architecture '$ARCH'"
	exit 1
fi
fi
rm -rf $PKGBUILD/BUILD $PKGBUILD/PKG

mkdir -p $PKGBUILD $PKGBUILD/BUILD $PKGBUILD/PKG $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.gz
rm -f $RPMBUILD/BUILD/wolframe_$VERSION.tar.gz

make distclean
mkdir /tmp/wolframe-$VERSION
cp -a * /tmp/wolframe-$VERSION
cd /tmp
tar zcf wolframe-$VERSION.tar.gz wolframe-$VERSION
cd -
mv /tmp/wolframe-$VERSION.tar.gz .
rm -rf /tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.gz $PKGBUILD/BUILD/.
cd $PKGBUILD/BUILD
tar zxf wolframe-$VERSION.tar.gz
cd wolframe-$VERSION

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	libdir=$LIBDIR DEFAULT_MODULE_LOAD_DIR=$LIBDIR/wolframe/modules

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	DESTDIR=$PKGBUILD/PKG install sysconfdir=/etc libdir=$LIBDIR

cd docs; make DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..

mkdir $PKGBUILD/PKG/install
cp packaging/slackware/slack-desc $PKGBUILD/PKG/install/.
#cp packaging/slackware/doinst-$ARCH.sh $PKGBUILD/PKG/install/.
cd $PKGBUILD/PKG
#makepkg -l y -c n $PKGBUILD/PKGS/$ARCH/wolframe-$VERSION.tgz
makepkg -c n $PKGBUILD/PKGS/$ARCH/wolframe-$VERSION.tgz

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build done."
