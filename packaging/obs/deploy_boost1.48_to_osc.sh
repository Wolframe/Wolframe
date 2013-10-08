#!/bin/sh

OSC_HOME=$HOME/home:andreas_baumann/boost1.48

( cd packaging/obs/boost1.48 && tar zcvf ../../../boost1.48_1.48.0.debian.tar.gz debian )
cp boost1.48_1.48.0.debian.tar.gz $OSC_HOME/.

SIZE=`stat -c '%s' $OSC_HOME/boost1.48_1.48.0.debian.tar.gz`
CHKSUM=`md5sum $OSC_HOME/boost1.48_1.48.0.debian.tar.gz  | cut -f 1 -d' '`

cat packaging/obs/boost1.48/boost1.48.dsc > $OSC_HOME/boost1.48.dsc
echo " $CHKSUM $SIZE boost1.48_1.48.0.debian.tar.gz" >> $OSC_HOME/boost1.48.dsc
