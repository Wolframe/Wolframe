#!/bin/sh

VERSION=0.0.1
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

make distclean
mkdir /var/tmp/wolframe-$VERSION
cp -a * /var/tmp/wolframe-$VERSION
cd /var/tmp
tar zcf wolframe-$VERSION.tar.gz wolframe-$VERSION
cd -
mv /var/tmp/wolframe-$VERSION.tar.gz .
rm -rf /var/tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.gz $PKGBUILD/BUILD/.
cp packaging/archlinux/* $PKGBUILD/BUILD/.

sed -i "s/make test/make testreport/g" $PKGBUILD/BUILD/PKGBUILD

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
