#!/bin/sh

OSC_HOME=$HOME/home:wolframe_user/libhpdf221

if test "x$TMPDIR" = "x"; then
	TMPDIR=/tmp
fi

cp packaging/obs/libhpdf221/libhpdf221-*.dsc $OSC_HOME
for i in `ls $OSC_HOME/libhpdf221-*.dsc`; do
	OS_ORIG=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//'`
	OS=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//' | tr -d '_'`
	rm -rf $OSC_HOME/libhpdf_2.2.1-$OS.debian.tar.gz
	rm -rf $TMPDIR/debian
	cp -a packaging/obs/libhpdf221/debian-$OS_ORIG $TMPDIR/.
	OLDDIR=$PWD
	cd $TMPDIR
	mv debian-$OS_ORIG debian
	tar zcf $TMPDIR/libhpdf_2.2.1-$OS.debian.tar.gz debian
	cd $OLDDIR
	rm -rf $TMPDIR/debian
	mv $TMPDIR/libhpdf_2.2.1-$OS.debian.tar.gz $OSC_HOME
	SIZE=`stat -c '%s' $OSC_HOME/libhpdf_2.2.1-$OS.debian.tar.gz`
	CHKSUM=`md5sum $OSC_HOME/libhpdf_2.2.1-$OS.debian.tar.gz | cut -f 1 -d' '`
	echo " $CHKSUM $SIZE libhpdf_2.2.1-$OS.debian.tar.gz" >> $i
done
