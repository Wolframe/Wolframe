#!/bin/sh

VERSION=0.0.1
PKGBUILD=$HOME/solarisbuild
ORIG_ARCH=`uname -m`

check_for_errors( )
{
	RET=$?
	if test $RET -gt 0; then
		echo "Build failed."
		exit 1
	fi
}

rm -rf $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION

mkdir -p $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.gz
rm -f $PKGBUILD/BUILD/wolframe_$VERSION.tar.gz

gmake distclean
rm -rf /tmp/wolframe-$VERSION
mkdir /tmp/wolframe-$VERSION
cp -R * /tmp/wolframe-$VERSION
OLDPWD=$PWD
cd /tmp
tar cf - wolframe-$VERSION | compress -c > wolframe-$VERSION.tar.Z
cd $OLDPWD
mv /tmp/wolframe-$VERSION.tar.Z .
rm -rf /tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.Z $PKGBUILD/BUILD/.
cd $PKGBUILD/BUILD
uncompress -c wolframe-$VERSION.tar.Z | tar xvf -
cd wolframe-$VERSION

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man \
	help

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man \
	config
	
LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man \
	config

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man
check_for_errors

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man \
	test
check_for_errors

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.48.0 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/usr/csw/etc libdir=/usr/csw/lib \
	libdir=/usr/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	mandir=/usr/csw/man \
	DESTDIR=$PKGBUILD/PKG/wolframe-$VERSION \
	install
check_for_errors

# doxygen package currently broken
#cd docs; gmake DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..

# currently no Solaris packages
#cp packaging/netbsd/comment $PKGBUILD/PKG/wolframe-$VERSION/.
#cp packaging/netbsd/description $PKGBUILD/PKG/wolframe-$VERSION/.
#cp packaging/netbsd/packlist $PKGBUILD/PKG/wolframe-$VERSION/.
#cp packaging/netbsd/iscript $PKGBUILD/PKG/wolframe-$VERSION/.
#cp packaging/netbsd/dscript $PKGBUILD/PKG/wolframe-$VERSION/.
#cp packaging/netbsd/wolframe.conf $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/etc/wolframe/.
#mkdir -p $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d
#cp packaging/netbsd/wolframed $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d/.
#chmod 0775 $PKGBUILD/PKG/wolframe-$VERSION/usr/pkg/share/examples/rc.d/wolframed

#OPSYS=`uname -s`
#OS_VERSION=`uname -r`
#cat <<EOF > $PKGBUILD/PKG/wolframe-$VERSION/build-info
#MACHINE_ARCH=$PACK_ARCH
#OPSYS=$OPSYS
#OS_VERSION=$OS_VERSION
#PKGTOOLS_VERSION=20091115
#EOF

#cd $PKGBUILD/PKG/wolframe-$VERSION

#pkg_create -v -p . -I / \
#	-B build-info \
#	-c comment \
#	-d description \
#	-f packlist \
#	-i iscript \
#	-k dscript \
#	$PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$ARCH.tgz
#check_for_errors

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build succeeded."
exit 0
