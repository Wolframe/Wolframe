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
. $base/hashmap.inc

cd $OSC_HOME/$OSC_PROJECT
osc -q up
OSC_REVISION=`osc info | grep Revision | cut -f 2 -d ' '`

hinit states
OLDIFS=$IFS
osc results > /tmp/results.$$
while read -r PLATFORM ARCH STATUS; do
	hput states "${ARCH}_${PLATFORM}" $STATUS
done < /tmp/results.$$
rm /tmp/results.$$
IFS=$OLDIFS

# see if web data directory is available, this will be the model
# for all our web pages
if test ! -d $DATA_DIR; then
	mkdir $DATA_DIR
fi

REPO_CACHE=$CACHE_DIR/repo.cache
archs=`cat $REPO_CACHE | cut -f 2 | sort | uniq`
platforms=`cat $REPO_CACHE | cut -f 1 | sort | uniq`

# logfile per architecture, platform and revision
for arch in $archs; do
	for platform in $platforms; do
		DEST_DIR=$DATA_DIR/$OSC_REVISION/$arch/$platform
		LOG_FILE=$DEST_DIR/log.txt
		if test ! -d $DEST_DIR; then
			mkdir -p $DEST_DIR
		fi
		STATUS=`hget states ${arch}_${platform}`
		case $STATUS in
			failed|succeeded)
				if test ! -f $LOG_FILE; then
					echo "Getting build log for $OSC_REVISION, $arch, $platform.."
					osc buildlog $platform $arch > $LOG_FILE
				fi
				;;
			
			*)
				echo "Ignoring build log for  $OSC_REVISION, $arch, $platform becauseof status '$STATUS'.."
		esac
	done
done

for arch in $archs; do
	for platform in $platforms; do
		DEST_DIR=$DATA_DIR/$OSC_REVISION/$arch/$platform
		LOG_FILE=$DEST_DIR/log.txt
		XML_FILE=$DEST_DIR/log.xml
		STATUS=`hget states ${arch}_${platform}`
		case $STATUS in
			failed|succeeded)
				echo "Generating meta XML for $OSC_REVISION, $arch, $platform.."
				STATUS=`hget states ${arch}_${platform}`
				tail -n 25 < $LOG_FILE > /tmp/tail.$$
				TAIL=`cat /tmp/tail.$$ | sed -e 's~&~\&amp;~g' -e 's~<~\&lt;~g'  -e  's~>~\&gt;~g'`
				cat >$XML_FILE <<EOF
		<log>
			<revision>$OSC_REVISION</revision>
			<arch>$arch</arch>
			<platform>$platform</platform>
			<status>$STATUS</status>
			<tail>$TAIL</tail>
		</log>
EOF
				rm /tmp/tail.$$
				;;
			
		esac
	done
done

hdestroy states
     
echo "Done."
