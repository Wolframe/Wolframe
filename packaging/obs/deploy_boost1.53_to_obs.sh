#!/bin/sh

OSC_HOME=$HOME/home:andreas_baumann/boost1.53

if test "x$TMPDIR" = "x"; then
	TMPDIR=/tmp
fi

cp packaging/obs/boost1.53/boost1.53-*.dsc $OSC_HOME
for i in `ls $OSC_HOME/boost1.53-*.dsc`; do
	OS_ORIG=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//'`
	OS=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//' | tr -d '_'`
	rm -rf $OSC_HOME/boost1.53_1.53.0-$OS.debian.tar.gz
	rm -rf $TMPDIR/debian
	cp -a packaging/obs/boost1.53/debian-$OS_ORIG $TMPDIR/.
	OLDDIR=$PWD
	cd $TMPDIR
	mv debian-$OS_ORIG debian
	tar zcf $TMPDIR/boost1.53_1.53.0-$OS.debian.tar.gz debian
	cd $OLDDIR
	rm -rf $TMPDIR/debian
	mv $TMPDIR/boost1.53_1.53.0-$OS.debian.tar.gz $OSC_HOME
	SIZE=`stat -c '%s' $OSC_HOME/boost1.53_1.53.0-$OS.debian.tar.gz`
	CHKSUM=`md5sum $OSC_HOME/boost1.53_1.53.0-$OS.debian.tar.gz | cut -f 1 -d' '`
	echo " $CHKSUM $SIZE boost1.53_1.53.0-$OS.debian.tar.gz" >> $i
done
