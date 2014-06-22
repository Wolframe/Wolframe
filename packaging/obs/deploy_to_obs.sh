#!/bin/sh

VERSION=0.0.2
OSC_HOME=$HOME/home:wolframe_user/Wolframe

if test "x$TMPDIR" = "x"; then
	TMPDIR=/tmp
fi

# the original source tarball for RHEL/Centos/OpenSUSE
rm -f wolframe-$VERSION.tar.gz
make dist-gz
cp wolframe-$VERSION.tar.gz $OSC_HOME/wolframe_$VERSION.tar.gz

# the original source tarball for Debian/Ubuntu
cp wolframe-$VERSION.tar.gz $OSC_HOME/wolframe_$VERSION.orig.tar.gz

# the Redhat build script
cp packaging/redhat/wolframe.spec $OSC_HOME/wolframe.spec

# patches
cp packaging/obs/boost1.48/boost_1_48_0-gcc-compile.patch $OSC_HOME/.

# compute sizes of packages
SIZE=`stat -c '%s' $OSC_HOME/wolframe_$VERSION.orig.tar.gz`
CHKSUM=`md5sum $OSC_HOME/wolframe_$VERSION.orig.tar.gz | cut -f 1 -d' '`

# copy all Debian versions of the description files.
cp packaging/obs/wolframe*.dsc $OSC_HOME


# Append calculated MD5/sizes of Debian/Ubuntu-version specific patches.
# normally 'packaging/debian' is taken, but for the files in obs where
# there is a difference for a specific version of Debian/Ubuntu.
# Patch 'debian/changelog' and Version in dsc file to have a build number
# and avoid funny problems as mentioned in issue #89)
GIT_COMMIT_COUNT=`git rev-list HEAD --count`
for i in `ls $OSC_HOME/wolframe-*.dsc`; do
	echo " $CHKSUM $SIZE wolframe_$VERSION.orig.tar.gz" >> $i
	OS_ORIG=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//'`
	OS=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//' | tr -d '_'`
	rm -rf $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz
	rm -rf $TMPDIR/debian
	cp -a packaging/debian $TMPDIR/.
	test -f packaging/obs/control-$OS_ORIG && cp -a packaging/obs/control-$OS_ORIG $TMPDIR/debian/control
	test -f packaging/obs/rules-$OS_ORIG && cp -a packaging/obs/rules-$OS_ORIG $TMPDIR/debian/rules
#	sed -i "s/Version: *\([0-9.]*\)\-\(.*\)/Version: \1-\2~dev$OBS_DEB_VERSION/" $i
	sed -i "s/wolframe (\([0-9.]*\)-\([0-9]*\))/wolframe (\1-$GIT_COMMIT_COUNT)/" $TMPDIR/debian/changelog
	sed -i "s/\$(PARALLEL_BUILD) test/\${PARALLEL_BUILD} testreport/g" $TMPDIR/debian/rules
	OLDDIR=$PWD
	cd $TMPDIR
	tar zcf $TMPDIR/wolframe_$VERSION-$OS.debian.tar.gz debian
	cd $OLDDIR
	mv -f $TMPDIR/wolframe_$VERSION-$OS.debian.tar.gz $OSC_HOME/.
	DEBIAN_SIZE=`stat -c '%s' $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz`
	DEBIAN_CHKSUM=`md5sum  $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz | cut -f 1 -d' '`
	echo " $DEBIAN_CHKSUM $DEBIAN_SIZE wolframe_$VERSION-$OS.debian.tar.gz" >> $i
done

# Archlinux specific files
# patch 'PKGBUILD' pkgver for now for ArchLinux (see above for Debian/Ubuntu)
cat packaging/obs/PKGBUILD > $OSC_HOME/PKGBUILD
sed -i "s/pkgrel=.*/pkgrel=$GIT_COMMIT_COUNT/" $OSC_HOME/PKGBUILD
cp packaging/archlinux/wolframe.conf $OSC_HOME/wolframe.conf
cp packaging/archlinux/wolframed.service $OSC_HOME/wolframed.service
cp packaging/archlinux/wolframe.install $OSC_HOME/wolframe.install

CHKSUM2=`md5sum $OSC_HOME/wolframe.conf | cut -f 1 -d' '`
CHKSUM3=`md5sum $OSC_HOME/wolframed.service | cut -f 1 -d' '`

echo "md5sums=('$CHKSUM' '$CHKSUM2' '$CHKSUM3')" >> $OSC_HOME/PKGBUILD

# the revision of the git branch we are currently building (master hash at the moment)
git rev-parse HEAD > $OSC_HOME/GIT_VERSION

# patch 'test' target to 'testreport'
sed -i "s/make test/make testreport/g" $OSC_HOME/PKGBUILD
sed -i "s/make test/make testreport/g" $OSC_HOME/wolframe.spec
