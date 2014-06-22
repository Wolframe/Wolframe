#!/bin/sh

# Note: Create a ~/.rpmmacros to set number of CPUs for
# parallel building like this:
# %_smp_mflags -j24

# Of course you need build toold and rpm-build, also a ~/rpmbuild directory
# set up

# the Centos version is set below as 'centos_version 630' (OSB service
# linguo for Centos 6.3)

VERSION=0.0.2
RPMBUILD=$HOME/rpmbuild
#OSB_PLATFORM=

rm -rf $RPMBUILD/BUILDROOT $RPMBUILD/BUILD/wolframe-$VERSION \
	$RPMBUILD/BUILD/boost-1.48.0 $RPMBUILD/BUILD/libxml2-2.9.1 \
	$RPMBUILD/BUILD/libxslt-1.1.28 $RPMBUILD/BUILD/Python-3.3.2 \
	$RPMBUILD/RPMS/*/* \
	$RPMBUILD/SRPMS/* $RPMBUILD/SPECS/log \
	/tmp/boost-1.48.0 /tmp/libxml2-2.9.1 /tmp/libxslt-1.1.28 /tmp/Python-3.3.2

# for rhel5
rm -rf /var/tmp/wolframe-root/

rm -f wolframe-$VERSION.tar.gz
rm -f $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz

make \
	WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	dist-gz >/dev/null 2>&1

cp wolframe-$VERSION.tar.gz $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz
cp packaging/redhat/wolframe.spec $RPMBUILD/SPECS/wolframe.spec

if test ! -f $RPMBUILD/SOURCES/boost_1_48_0.tar.gz; then
	wget -O $RPMBUILD/SOURCES/boost_1_48_0.tar.gz \
		http://downloads.sourceforge.net/project/boost/boost/1.48.0/boost_1_48_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.48.0%2F&ts=1353483626&use_mirror=ignum
fi

if test ! -f $RPMBUILD/SOURCES/Python-3.3.2.tar.bz2; then
	wget --no-check-certificate -O $RPMBUILD/SOURCES/Python-3.3.2.tgz \
		http://www.python.org/ftp/python/3.3.2/Python-3.3.2.tgz
	gunzip $RPMBUILD/SOURCES/Python-3.3.2.tgz
	bzip2 $RPMBUILD/SOURCES/Python-3.3.2.tar
fi

if test ! -f $RPMBUILD/SOURCES/libxml2-2.9.1.tar.gz; then
	wget -O $RPMBUILD/SOURCES/libxml2-2.9.1.tar.gz \
		ftp://xmlsoft.org/libxml2/libxml2-2.9.1.tar.gz
fi

if test ! -f $RPMBUILD/SOURCES/libxslt-1.1.28.tar.gz; then
	wget -O $RPMBUILD/SOURCES/libxslt-1.1.28.tar.gz \
		ftp://xmlsoft.org/libxml2/libxslt-1.1.28.tar.gz
fi

cp packaging/obs/boost1.48/boost_1_48_0-gcc-compile.patch $RPMBUILD/SOURCES/.

cd $RPMBUILD/SPECS

# add Intel compiler to the path if we have one (Centos VMs with Intel CC only)
if test -f /opt/intel/bin/iccvars.sh; then
	MACHINE_ARCH=`uname -m`
	if test "$MACHINE_ARCH" = "x86_64"; then
		ICC_ARCH="intel64"
	else
		if test "$MACHINE_ARCH" = "i686"; then
			ICC_ARCH="ia32"
		else
			print "ERROR: Unknown Intel architecture $MACHIN_ARCH!"
			global_unlock
			exit 1
		fi
	fi
	. /opt/intel/bin/iccvars.sh $ICC_ARCH
	export CCACHE_CPP2=1
	export CC='ccache icc'
	export CXX='ccache icpc'	
else
	export CC='ccache gcc'
	export CXX='ccache g++'
fi
sed -i "s/make test/make testreport/g" wolframe.spec
rpmbuild -ba --define "$OSB_PLATFORM" wolframe.spec

RET=$?
if test $RET -eq 0; then
	echo "Build succeeded."
	exit 0
else
	echo "Build failed."
	exit 1
fi

