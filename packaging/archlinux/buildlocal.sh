#!/bin/sh

VERSION=0.0.3
PKGBUILD=$HOME/archbuild
ARCH=`uname -m`
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

GIT_COMMIT_COUNT=`git describe --long --tags | cut -f 2 -d -`
make distclean
mkdir /var/tmp/wolframe-$VERSION
cp -a * /var/tmp/wolframe-$VERSION
cd /var/tmp
sed -i "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD $GIT_COMMIT_COUNT/g" wolframe-$VERSION/include/wolframe.hpp
tar zcf wolframe-$VERSION.tar.gz wolframe-$VERSION
cd -
mv /var/tmp/wolframe-$VERSION.tar.gz .
rm -rf /var/tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.gz $PKGBUILD/BUILD/.
cp packaging/archlinux/* $PKGBUILD/BUILD/.

# enable test reporting (Wolframe and GTest)
sed -i "s/make test/make testreport/g" $PKGBUILD/BUILD/PKGBUILD
export WOLFRAME_TESTREPORT_DIR=$PKGBUILD/BUILD/src/wolframe-$VERSION/tests/reports/
export GTEST_OUTPUT=xml:$PKGBUILD/BUILD/src/wolframe-$VERSION/tests/reports/

cd $PKGBUILD/BUILD
sed -i 's|"http://sourceforge.net/projects/wolframe/files/.*"|"${pkgname}-${pkgver}.tar.gz"|g' PKGBUILD
makepkg --asroot -g >> PKGBUILD
makepkg --asroot 
check_for_errors

cp $PKGBUILD/BUILD/wolframe-*.pkg.tar.xz $PKGBUILD/PKGS/$ARCH/.

# rm -rf $PKGBUILD/BUILD
# rm -rf $PKGBUILD/PKG

echo "Build succeeded."
exit 0
