#!/bin/sh

OSC_HOME=$HOME/home:andreas_baumann/boost1.49

( cd contrib/osc/boost1.49 && tar zcvf ../../../boost1.49_1.49.0.debian.tar.gz debian )
cp boost1.49_1.49.0.debian.tar.gz $OSC_HOME/.

SIZE=`stat -c '%s' $OSC_HOME/boost1.49_1.49.0.debian.tar.gz`
CHKSUM=`md5sum $OSC_HOME/boost1.49_1.49.0.debian.tar.gz  | cut -f 1 -d' '`

cat contrib/osc/boost1.49/boost1.49.dsc > $OSC_HOME/boost1.49.dsc
echo " $CHKSUM $SIZE boost1.49_1.49.0.debian.tar.gz" >> $OSC_HOME/boost1.49.dsc
