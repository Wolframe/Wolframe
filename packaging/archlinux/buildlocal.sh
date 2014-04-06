#!/bin/sh

VERSION=0.0.1
PKGBUILD=$HOME/archbuild
ARCH=`uname -m`
if test "x$ARCH" = "xx86_64"; then
	LIBDIR="/usr/lib"
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

make distclean
mkdir /tmp/wolframe-$VERSION
cp -a * /tmp/wolframe-$VERSION
cd /tmp
tar zcf wolframe-$VERSION.tar.gz wolframe-$VERSION
cd -
mv /tmp/wolframe-$VERSION.tar.gz .
rm -rf /tmp/wolframe-$VERSION

cp wolframe-$VERSION.tar.gz $PKGBUILD/BUILD/.
cp packaging/archlinux/* $PKGBUILD/BUILD/.

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
