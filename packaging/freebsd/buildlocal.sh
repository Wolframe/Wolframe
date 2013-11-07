#!/bin/sh

VERSION=0.0.1
PKGBUILD=$HOME/bsdbuild
ORIG_ARCH=`uname -m`
if test "x$ORIG_ARCH" = "xamd64"; then
	ARCH="x86_64"
	FREEIMAGE="WITH_LOCAL_FREEIMAGE=1"
else
if test "x$ORIG_ARCH" = "xi386"; then
	ARCH="i686"
	# Freeimage is broken on 32-bit FreeBSD (because of gcc)
	FREEIMAGE=""
else
	echo "ERROR: Unknown slackware architecture '$ORIG_ARCH'"
	exit 1
fi
fi

OS_VERSION=`uname -r`
case $OS_VERSION in
	8.3*)
		FREEIMAGE=""
		;;
esac

rm -rf $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION

mkdir -p $PKGBUILD $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.gz
rm -f $PKGBUILD/BUILD/wolframe_$VERSION.tar.gz

gmake distclean
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

gmake WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 $FREEIMAGE \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/local/lib/wolframe" \
	libdir=/usr/local/lib DEFAULT_MODULE_LOAD_DIR=/usr/local/lib/wolframe/modules

gmake WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 $FREEIMAGE \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/local/lib/wolframe" \
	test 

gmake WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 $FREEIMAGE \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/local/lib/wolframe" \
	prefix=/usr/local \
	DESTDIR=$PKGBUILD/PKG/wolframe-$VERSION install sysconfdir=/usr/local/etc libdir=/usr/local/lib

# doxygen package currently broken
#cd docs; gmake DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..

cp packaging/freebsd/comment $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/freebsd/description $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/freebsd/packlist $PKGBUILD/PKG/wolframe-$VERSION/.
if test "x$FREEIMAGE" = "xWITH_LOCAL_FREEIMAGE=1"; then
	cat packaging/freebsd/packlist.freeimage >> $PKGBUILD/PKG/wolframe-$VERSION/packlist
fi
cp packaging/freebsd/iscript $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/freebsd/dscript $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/freebsd/wolframe.conf $PKGBUILD/PKG/wolframe-$VERSION/usr/local/etc/wolframe/.
mkdir $PKGBUILD/PKG/wolframe-$VERSION/usr/local/etc/rc.d
cp packaging/freebsd/wolframed $PKGBUILD/PKG/wolframe-$VERSION/usr/local/etc/rc.d/.
chmod 0775 $PKGBUILD/PKG/wolframe-$VERSION/usr/local/etc/rc.d/wolframed

cd $PKGBUILD

pkg_create -S $PKGBUILD -z -v \
	-c PKG/wolframe-$VERSION/comment \
	-d PKG/wolframe-$VERSION/description \
	-f PKG/wolframe-$VERSION/packlist \
	-i PKG/wolframe-$VERSION/iscript \
	-k PKG/wolframe-$VERSION/dscript \
	$PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$ARCH.tgz

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build done."