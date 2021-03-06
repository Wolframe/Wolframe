#!/bin/sh

VERSION=0.0.3
PKGBUILD=$HOME/bsdbuild
ORIG_ARCH=`uname -m`
if test "x$ORIG_ARCH" = "xamd64"; then
	ARCH="x86_64"
	PACK_ARCH="x86_64"
else
if test "x$ORIG_ARCH" = "xi386"; then
	ARCH="i686"
	PACK_ARCH="i386"
else
	echo "ERROR: Unknown NetBSD architecture '$ORIG_ARCH'"
	exit 1
fi
fi

check_for_errors( )
{
	RET=$?
	if test $RET -gt 0; then
		echo "Build failed."
		exit 1
	fi
}

rm -rf $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION

mkdir -p $PKGBUILD $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.gz
rm -f $PKGBUILD/BUILD/wolframe_$VERSION.tar.gz

GIT_COMMIT_COUNT=`git describe --long --tags | cut -f 2 -d -`
gmake distclean
mkdir /tmp/wolframe-$VERSION
cp -a * /tmp/wolframe-$VERSION
cd /tmp
sed "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD $GIT_COMMIT_COUNT/g" wolframe-$VERSION/include/wolframe.hpp > _tmp
mv -f _tmp wolframe-$VERSION/include/wolframe.hpp
tar zcf wolframe-$VERSION.tar.gz wolframe-$VERSION
cd -
mv /tmp/wolframe-$VERSION.tar.gz .
rm -rf /tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.gz $PKGBUILD/BUILD/.
cd $PKGBUILD/BUILD
tar zxf wolframe-$VERSION.tar.gz
cd wolframe-$VERSION

gmake WITH_SSL=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/pkg/lib/wolframe,--enable-new-dtags" \
	prefix=/usr/pkg \
	sysconfdir=/usr/pkg/etc libdir=/usr/pkg/lib \
	libdir=/usr/pkg/lib DEFAULT_MODULE_LOAD_DIR=/usr/pkg/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/usr/pkg/etc/wolframe/wolframe.conf \
	mandir=/usr/pkg/man \
	help

gmake WITH_SSL=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/pkg/lib/wolframe,--enable-new-dtags" \
	prefix=/usr/pkg \
	sysconfdir=/usr/pkg/etc libdir=/usr/pkg/lib \
	libdir=/usr/pkg/lib DEFAULT_MODULE_LOAD_DIR=/usr/pkg/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/usr/pkg/etc/wolframe/wolframe.conf \
	mandir=/usr/pkg/man \
	config

gmake WITH_SSL=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/pkg/lib/wolframe,--enable-new-dtags" \
	prefix=/usr/pkg \
	sysconfdir=/usr/pkg/etc libdir=/usr/pkg/lib \
	libdir=/usr/pkg/lib DEFAULT_MODULE_LOAD_DIR=/usr/pkg/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/usr/pkg/etc/wolframe/wolframe.conf \
	mandir=/usr/pkg/man
check_for_errors

# testing breaks at least in boost-locale and some xml filters for now
gmake WITH_SSL=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/pkg/lib/wolframe,--enable-new-dtags" \
	prefix=/usr/pkg \
	sysconfdir=/usr/pkg/etc libdir=/usr/pkg/lib \
	libdir=/usr/pkg/lib DEFAULT_MODULE_LOAD_DIR=/usr/pkg/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/usr/pkg/etc/wolframe/wolframe.conf \
	mandir=/usr/pkg/man \
	testreport 
check_for_errors

gmake WITH_SSL=1 WITH_SASL=1 WITH_LOCAL_SQLITE3=1 \
	WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
	WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 RELEASE=1 \
	CC='ccache gcc' CXX='ccache g++' \
	LDFLAGS="-Wl,-rpath=/usr/pkg/lib/wolframe,--enable-new-dtags" \
	prefix=/usr/pkg \
	sysconfdir=/usr/pkg/etc libdir=/usr/pkg/lib \
	libdir=/usr/pkg/lib DEFAULT_MODULE_LOAD_DIR=/usr/pkg/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/usr/pkg/etc/wolframe/wolframe.conf \
	mandir=/usr/pkg/man \
	DESTDIR=$PKGBUILD/PKG/wolframe-$VERSION \
	install
check_for_errors

# doxygen package currently broken
#cd docs; gmake DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..

cp packaging/netbsd/comment $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/netbsd/description $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/netbsd/packlist $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/netbsd/iscript $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/netbsd/dscript $PKGBUILD/PKG/wolframe-$VERSION/.
mkdir -p $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d
cp packaging/netbsd/wolframed $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d/.
chmod 0775 $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d/wolframed

OPSYS=`uname -s`
OS_VERSION=`uname -r`
cat <<EOF > $PKGBUILD/PKG/wolframe-$VERSION/build-info
MACHINE_ARCH=$PACK_ARCH
OPSYS=$OPSYS
OS_VERSION=$OS_VERSION
PKGTOOLS_VERSION=20091115
EOF

cd $PKGBUILD/PKG/wolframe-$VERSION

pkg_create -v -p . -I / \
	-B build-info \
	-c comment \
	-d description \
	-f packlist \
	-i iscript \
	-k dscript \
	$PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$ARCH.tgz
check_for_errors

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build succeeded."
exit 0
