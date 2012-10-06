#!/bin/sh

VERSION=0.0.1
OSC_HOME=$HOME/home:andreas_baumann/boost1.48

( cd contrib/osc/boost && tar zcvf ../../../boost1.48_1.48.0.debian.tar.gz debian )
cp boost1.48_1.48.0.debian.tar.gz $OSC_HOME/.

SIZE=`stat -c '%s' $OSC_HOME/boost1.48_1.48.0.debian.tar.gz`
CHKSUM=`md5sum $OSC_HOME/boost1.48_1.48.0.debian.tar.gz  | cut -f 1 -d' '`

cat contrib/osc/boost/boost1.48.dsc > $OSC_HOME/boost1.48.dsc
echo " $CHKSUM $SIZE boost1.48_1.48.0.debian.tar.gz" >> $OSC_HOME/boost1.48.dsc
