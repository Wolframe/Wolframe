#!/bin/sh

PROJECT=wolframe
VERSION=0.0.1

PACKAGE_ROOT=/mnt/packages
SOURCEFORGE_ROOT=/mnt/sf

PLATFORMS="Slackware_14 FreeBSD_9 FreeBSD_8 NetBSD_6 Scientific_Linux-6 Scientific_Linux-5"
ARCHS="i586 x86_64"

BASE=$PWD

create_sf_dir( )
{
	DIR=$1
	test -d $SOURCEFORGE_ROOT/$DIR || mkdir $SOURCEFORGE_ROOT/$DIR
}

map_platform_and_arch( )
{
	case $PLATFORM in
		RedHat_RHEL-5)		NICE_PLATFORM="RHEL-5"			;;
		RedHat_RHEL-6)		NICE_PLATFORM="RHEL-6"			;;
		CentOS_CentOS-5)	NICE_PLATFORM="CentOS-5"		;;
		CentOS_CentOS-6)	NICE_PLATFORM="CentOS-6"		;;
		Fedora_19)		NICE_PLATFORM="Fedora-19"		;;
		Fedora_18)		NICE_PLATFORM="Fedora-18"		;;
		Debian_7.0)		NICE_PLATFORM="Debian-7"		;;
		Debian_6.0)		NICE_PLATFORM="Debian-6"		;;
		Arch_Extra)		NICE_PLATFORM="ArchLinux"		;;
		SLE_11_SP2)		NICE_PLATFORM="SLES-11_SP2"		;;
		SLE_11_SP3)		NICE_PLATFORM="SLES-11_SP3"		;;
		openSUSE_12.3)		NICE_PLATFORM="openSUSE-12.3"		;;
		openSUSE_13.1 )		NICE_PLATFORM="openSUSE-13.1"		;;
		xUbuntu_10.04)		NICE_PLATFORM="Ubuntu-10.04_LTS"	;;
		xUbuntu_12.04)		NICE_PLATFORM="Ubuntu-12.04_LTS"	;;
		xUbuntu_12.10)		NICE_PLATFORM="Ubuntu-12.10"		;;
		xUbuntu_13.04)		NICE_PLATFORM="Ubuntu-13.04"		;;
		xUbuntu_13.10)		NICE_PLATFORM="Ubuntu-13.10"		;;
		Slackware_14)		NICE_PLATFORM="Slackware-14"		;;
		FreeBSD_9)		NICE_PLATFORM="FreeBSD-9"		;;
		FreeBSD_8)		NICE_PLATFORM="FreeBSD-8"		;;
		NetBSD_6)		NICE_PLATFORM="NetBSD-6"		;;
		Scientific_Linux-6)	NICE_PLATFORM="Scientific_Linux-6"	;;
		Scientific_Linux-5)	NICE_PLATFORM="Scientific_Linux-5"	;;
		*)			NICE_PLATFORM=$PLATFORM			;;
	esac
	
	case $ARCH in
		i586)			NICE_ARCH="i386"			;;
		x86_64)			NICE_ARCH="x86_64"			;;
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
