#!/bin/sh

OSC_HOME=$HOME/home:andreas_baumann/boost1.49

cp packaging/obs/boost1.49/boost1.49-*.dsc $OSC_HOME
for i in `ls $OSC_HOME/boost1.49-*.dsc`; do
	OS_ORIG=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//'`
	OS=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//' | tr -d '_'`
	rm -rf $OSC_HOME/boost1.49_1.49.0-$OS.debian.tar.gz
	rm -rf /tmp/debian
	cp -a packaging/obs/boost1.49/debian-$OS_ORIG /tmp/.
	OLDDIR=$PWD
	cd /tmp
	mv debian-$OS_ORIG debian
	tar zcf /tmp/boost1.49_1.49.0-$OS.debian.tar.gz debian
	cd $OLDDIR
	rm -rf /tmp/debian
	mv /tmp/boost1.49_1.49.0-$OS.debian.tar.gz $OSC_HOME
	SIZE=`stat -c '%s' $OSC_HOME/boost1.49_1.49.0-$OS.debian.tar.gz`
	CHKSUM=`md5sum $OSC_HOME/boost1.49_1.49.0-$OS.debian.tar.gz | cut -f 1 -d' '`
	echo " $CHKSUM $SIZE boost1.49_1.49.0-$OS.debian.tar.gz" >> $i
done
