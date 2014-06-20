#!/bin/sh

PROJECT=wolframe
VERSION=0.0.2

PACKAGE_ROOT=/mnt/packages
SOURCEFORGE_ROOT=/mnt/sf

PLATFORMS="Slackware_14 FreeBSD_10 FreeBSD_9 FreeBSD_8 NetBSD_6 Scientific_Linux-6 Scientific_Linux-5 Solaris_10"
#PLATFORMS="Solaris_10"
ARCHS="i586 x86_64"
#ARCHS="sun4u"

BASE=$PWD

create_sf_dir( )
{
	DIR=$1
	test -d $SOURCEFORGE_ROOT/$DIR || mkdir $SOURCEFORGE_ROOT/$DIR
}

map_platform_and_arch( )
{
	case $PLATFORM in
		Slackware_14)		NICE_PLATFORM="Slackware-14"		;;
		FreeBSD_10)		NICE_PLATFORM="FreeBSD-10"		;;
		FreeBSD_9)		NICE_PLATFORM="FreeBSD-9"		;;
		FreeBSD_8)		NICE_PLATFORM="FreeBSD-8"		;;
		NetBSD_6)		NICE_PLATFORM="NetBSD-6"		;;
		Scientific_Linux-6)	NICE_PLATFORM="Scientific_Linux-6"	;;
		Scientific_Linux-5)	NICE_PLATFORM="Scientific_Linux-5"	;;
		Solaris_10)		NICE_PLATFORM="Solaris-10"		;;
		*)			NICE_PLATFORM=$PLATFORM			;;
	esac
	
	case $ARCH in
		i586)			NICE_ARCH="i386"			;;
		x86_64)			NICE_ARCH="x86_64"			;;
		sun4u)			NICE_ARCH="sparc"			;;
		*)			NICE_ARCH=$ARCH				;;
	esac
}

test -d _work || mkdir _work
cd _work

echo "Updating locally built Sourceforge binaries for project '$PROJECT', version '$VERSION'.."

echo "Creating directory layout for new version '$VERSION' on Sourceforge.."
create_sf_dir $PROJECT-binaries 
create_sf_dir $PROJECT-binaries/$VERSION

for PLATFORM in $PLATFORMS; do
	for ARCH in $ARCHS; do
		map_platform_and_arch
	
		echo "Creating directories for '$VERSION', '$NICE_PLATFORM', '$NICE_ARCH'.."
		create_sf_dir "$PROJECT-binaries/$VERSION/$NICE_PLATFORM"
		create_sf_dir "$PROJECT-binaries/$VERSION/$NICE_PLATFORM/$NICE_ARCH"

		echo "Copying packages for '$PLATFORM', '$ARCH'.."
		destdir="$SOURCEFORGE_ROOT/$PROJECT-binaries/$VERSION/$NICE_PLATFORM/$NICE_ARCH"
		srcdir="$PACKAGE_ROOT/$PLATFORM/$ARCH"
		cp -av $srcdir/*${PROJECT}* $destdir
	done		
done

exit
