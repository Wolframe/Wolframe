#!/bin/sh

VERSION=0.0.1
RPMBUILD=$HOME/rpmbuild

rm -f wolframe-$VERSION.tar.gz
rm -f $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz

make \
	WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_ICU=1 dist-gz

cp wolframe-$VERSION.tar.gz $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz
cp redhat/wolframe.spec $RPMBUILD/SPECS/wolframe.spec

if test ! -f $RPMBUILD/SOURCES/boost_1_48_0.tar.gz; then
	wget -O $RPMBUILD/SOURCES/boost_1_48_0.tar.gz \
		http://downloads.sourceforge.net/project/boost/boost/1.48.0/boost_1_48_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.48.0%2F&ts=1353483626&use_mirror=ignum
fi
 
cp contrib/osc/boost/boost_1_48_0-gcc-compile.patch $RPMBUILD/SOURCES/.

cd $RPMBUILD/SPECS

echo "Building started, check with 'tail -f $RPMBUILD/SPECS/log'."

rpmbuild -ba --define 'rhel_version 630' wolframe.spec > log 2>&1

echo "Build done."
