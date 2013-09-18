#!/bin/sh

case "$0" in
	/*)
		base=`dirname $0`
		;;
	
	*)
		base=`pwd`/`dirname $0`
		;;
esac
                        
. $base/config

cd $OSC_HOME/$OSC_PROJECT

echo "Updating metadata for project '$OSC_PROJECT'.."

# see if web data directory is available, this will be the model
# for all our web pages
if test ! -d $DATA_DIR; then
	mkdir $DATA_DIR
fi

# make sure the cache dir exists
if test ! -d $CACHE_DIR; then
	mkdir $CACHE_DIR
fi

# cache repositories and architectures
REPO_CACHE=$CACHE_DIR/repo.cache
if test ! -f $REPO_CACHE; then
	osc repos | tr -s ' ' "\t" > $REPO_CACHE
fi

if test ! -d $XSLT_DIR; then
	echo "ERROR: No XSLT directory at '$XSLT_DIR'1!" 1>2
	exit 1
fi

# architectures we compile on
ARCH_FILE=$XSLT_DIR/archs.xml
if test ! -f $ARCH_FILE; then
	archs=`cat $REPO_CACHE | cut -f 2 | sort | uniq`
	echo "<archs>" > $ARCH_FILE
	for arch in $archs; do
		echo "	<arch>$arch</arch>" >> $ARCH_FILE
	done
	echo "</archs>" >> $ARCH_FILE
fi

# platforms we compile on
PLATFORM_FILE=$XSLT_DIR/platforms.xml
if test ! -f $PLATFORM_FILE; then
	platforms=`cat $REPO_CACHE | cut -f 1 | sort | uniq`
	echo "<platforms>" > $PLATFORM_FILE
	for platform in $platforms; do
		echo "	<platform>$platform</platform>" >> $PLATFORM_FILE
	done
	echo "</platforms>" >> $PLATFORM_FILE
fi

echo "Done."
