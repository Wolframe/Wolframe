#!/bin/sh

if test $# != 1; then
	echo "usage: setversion.sh <VERSION>"
	exit 1
fi

FULL_VERSION=$1

MAJOR_VERSION=`echo $FULL_VERSION | cut -f 1 -d .`
MINOR_VERSION=`echo $FULL_VERSION | cut -f 2 -d .`
REVISION=`echo $FULL_VERSION | cut -f 3 -d .`
BUILD=`echo $FULL_VERSION | cut -f 4 -d .`

VERSION="$MAJOR_VERSION.$MINOR_VERSION.$REVISION"

# Set PACKAGE_VERSION in makefiles (make dist and building)
sed -i "s/^PACKAGE_VERSION.*=.*/PACKAGE_VERSION = $VERSION/g" GNUmakefile
sed -i "s/^PACKAGE_VERSION.*=.*/PACKAGE_VERSION = $VERSION/g" Makefile.W32

# _VERSION constants
sed -i "s/^#define WOLFRAME_MAJOR_VERSION.*/#define WOLFRAME_MAJOR_VERSION	$MAJOR_VERSION/g" include/wolframe.hpp
sed -i "s/^#define WOLFRAME_MINOR_VERSION.*/#define WOLFRAME_MINOR_VERSION	$MINOR_VERSION/g" include/wolframe.hpp
sed -i "s/^#define WOLFRAME_REVISION.*/#define WOLFRAME_REVISION	$REVISION/g" include/wolframe.hpp
sed -i "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD		$BUILD/g" include/wolframe.hpp

# package metadata
sed -i "s/pkgver=.*/pkgver=$VERSION/g" packaging/archlinux/PKGBUILD
sed -i "s/^Version:.*/Version: $VERSION/g" packaging/redhat/wolframe.spec
sed -i "s/^wolframe-.*:/wolframe-$VERSION:/g" packaging/slackware/slack-desc
sed -i "s/<?define.*ProductVersion.*=.*\"[^\"]*\".*?>/<?define ProductVersion = \"$VERSION\" ?>/g" packaging/windows/wolframe.wxs
sed -i "s/^VERSION=.*/VERSION=\"$VERSION\"/g" packaging/solaris/pkginfo
sed -i "s/^version:.*/version: $VERSION/g" packaging/freebsd/+MANIFEST
sed -i "s/^wolframe - Wolframe server and module, .*/wolframe - Wolframe server and module, $VERSION/g" packaging/freebsd/comment
sed -i "s/^@name wolframe-.*/@name wolframe-$VERSION/g" packaging/freebsd/packlist
sed -i "s/^@srcdir PKG\/wolframe-.*/@srcdir PKG\/wolframe-$VERSION/g" packaging/freebsd/packlist
sed -i "s/^wolframe - Wolframe server and module, .*/wolframe - Wolframe server and module, $VERSION/g" packaging/netbsd/comment
sed -i "s/^@name wolframe-.*/@name wolframe-$VERSION/g" packaging/netbsd/packlist
sed -i "s/^@srcdir PKG\/wolframe-.*/@srcdir PKG\/wolframe-$VERSION/g" packaging/netbsd/packlist
sed -i "s/^udeb: libwolframe [0-9]* wolframe (>= .*)/udeb: libwolframe $MAJOR_VERSION wolframe (>= $VERSION)/g" packaging/debian/shlibs

# local package build scripts
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/redhat/buildlocal.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/archlinux/buildlocal.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/slackware/buildlocal.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/solaris/buildlocal.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/freebsd/buildlocal.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/netbsd/buildlocal.sh

# OpenSuse build
sed -i "s/pkgver=.*/pkgver=$VERSION/g" packaging/obs/PKGBUILD
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/obs/release_osb_binaries.sh
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/obs/deploy_to_obs.sh
for i in `find packaging/obs/wolframe-*.dsc`; do
	sed -i "s/^Version: [^\-]*\-\(.*\)/Version: $VERSION-\1/g" $i
done

# helper scripts
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/tools/freebsd.test.packlist
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/tools/netbsd.test.packlist
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/tools/solaris.test.pkgproto

# package releasing
sed -i "s/^VERSION=.*/VERSION=$VERSION/g" packaging/obs/release_local_binaries.sh

# documentation
echo $VERSION > docs/WolframeVersion.txt
echo $VERSION > docs/InstallationVersion.txt
