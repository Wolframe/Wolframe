#!/bin/sh

VERSION=0.0.2
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

check_for_errors( )
{
	RET=$?
	if test $RET -gt 0; then
		echo "Build failed."
		exit 1
	fi
}

mkdir -p $PKGBUILD $PKGBUILD/BUILD $PKGBUILD/PKG $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.gz
rm -f $PKGBUILD/BUILD/wolframe_$VERSION.tar.gz

GIT_COMMIT_COUNT=`git describe --long --tags | cut -f 2 -d -`
make distclean
mkdir /tmp/wolframe-$VERSION
cp -a * /tmp/wolframe-$VERSION
cd /tmp
sed -i "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD $GIT_COMMIT_COUNT/g" wolframe-$VERSION/include/wolframe.hpp
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
	LDFLAGS="-Wl,-rpath=$LIBDIR/wolframe,--enable-new-dtags" \
	libdir=$LIBDIR DEFAULT_MODULE_LOAD_DIR=$LIBDIR/wolframe/modules \
	help

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=$LIBDIR/wolframe,--enable-new-dtags" \
	libdir=$LIBDIR DEFAULT_MODULE_LOAD_DIR=$LIBDIR/wolframe/modules \
	config

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=$LIBDIR/wolframe,--enable-new-dtags" \
	libdir=$LIBDIR DEFAULT_MODULE_LOAD_DIR=$LIBDIR/wolframe/modules
check_for_errors

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=$LIBDIR/wolframe,--enable-new-dtags" \
	libdir=$LIBDIR DEFAULT_MODULE_LOAD_DIR=$LIBDIR/wolframe/modules \
	testreport
check_for_errors

make WITH_SSL=1 WITH_EXPECT=1 WITH_SASL=1 WITH_SYSTEM_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 PGSQL_DIR=/usr/local/pgsql \
	WITH_TEXTWOLF=1 WITH_CJSON=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=$LIBDIR/wolframe,--enable-new-dtags" \
	DESTDIR=$PKGBUILD/PKG install sysconfdir=/etc libdir=$LIBDIR
check_for_errors

cd docs; make DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..
check_for_errors

mkdir $PKGBUILD/PKG/install
cp packaging/slackware/slack-desc $PKGBUILD/PKG/install/.
cp packaging/slackware/doinst.sh $PKGBUILD/PKG/install/.
mkdir $PKGBUILD/PKG/etc/rc.d
cp packaging/slackware/rc.wolframed $PKGBUILD/PKG/etc/rc.d/.
chmod 0775 $PKGBUILD/PKG/etc/rc.d/rc.wolframed

cd $PKGBUILD/PKG
makepkg -l y -c n $PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$ARCH.tgz
check_for_errors

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build succeeded."
exit 0
