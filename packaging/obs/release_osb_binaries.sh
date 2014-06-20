#!/bin/sh

OBS_PROJECT=home:wolframe_user
OBS_PACKAGE=Wolframe
PROJECT=wolframe
VERSION=0.0.1

SOURCEFORGE_ROOT=/mnt/sf

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
		Fedora_20)		NICE_PLATFORM="Fedora-20"		;;
		Debian_7.0)		NICE_PLATFORM="Debian-7"		;;
		Debian_6.0)		NICE_PLATFORM="Debian-6"		;;
		Arch_Extra)		NICE_PLATFORM="ArchLinux"		;;
		SLE_11_SP2)		NICE_PLATFORM="SLES-11_SP2"		;;
		SLE_11_SP3)		NICE_PLATFORM="SLES-11_SP3"		;;
		openSUSE_12.3)		NICE_PLATFORM="openSUSE-12.3"		;;
		openSUSE_13.1)		NICE_PLATFORM="openSUSE-13.1"		;;
		xUbuntu_10.04)		NICE_PLATFORM="Ubuntu-10.04_LTS"	;;
		xUbuntu_12.04)		NICE_PLATFORM="Ubuntu-12.04_LTS"	;;
		xUbuntu_13.10)		NICE_PLATFORM="Ubuntu-13.10"		;;
		xUbuntu_14.04)		NICE_PLATFORM="Ubuntu-14.04_LTS"		;;
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

echo "Updating OSC work directory and metadata.."

if test ! -d $OBS_PROJECT/$OBS_PACKAGE; then
	osc co $OBS_PROJECT/$OBS_PACKAGE
else
	cd $OBS_PROJECT/$OBS_PACKAGE
	osc up >/dev/null 2>&1
fi

OBS_REVISION=`osc info | grep Revision | cut -f 2 -d ' '`

# cache repositories and architectures
REPO_CACHE=$BASE/_work/repo.cache
if test ! -f $REPO_CACHE; then
	cd $BASE/_work/$OBS_PROJECT
	osc repos | tr -s ' ' "\t" > $REPO_CACHE
fi

echo "Updating Sourceforge binaries for project '$OBS_PACKAGE', version '$VERSION'.."

cd $BASE/_work/$OBS_PROJECT/$OBS_PACKAGE

# check first if we are currently building, in this
# case wait and don't download binaries right now
OLDIFS=$IFS
osc results > /tmp/results.$$
while read -r PLATFORM ARCH STATUS; do
	if test $STATUS != "succeeded" -a $STATUS != "failed" -a $STATUS != 'disabled'; then
		echo "Not updating binaries now, platform $PLATFORM, arch $ARCH is still building.."
		exit 0
	fi
done < /tmp/results.$$
rm /tmp/results.$$
IFS=$OLDIFS

echo "Creating directory layout for new version '$VERSION' on Sourceforge.."
create_sf_dir $PROJECT-binaries 
create_sf_dir $PROJECT-binaries/$VERSION

cd $BASE/_work/$OBS_PROJECT/$OBS_PACKAGE
osc results > /tmp/results.$$
OLDIFS=$IFS
while read -r PLATFORM ARCH STATUS; do
	map_platform_and_arch
	
	echo "Creating directories for '$VERSION', '$NICE_PLATFORM', '$NICE_ARCH'.."
	create_sf_dir "$PROJECT-binaries/$VERSION/$NICE_PLATFORM"
	create_sf_dir "$PROJECT-binaries/$VERSION/$NICE_PLATFORM/$NICE_ARCH"

	echo "Getting packages for '$PLATFORM', '$ARCH'.."
	rm -rf binaries
	osc getbinaries $PLATFORM $ARCH
	destdir="$SOURCEFORGE_ROOT/$PROJECT-binaries/$VERSION/$NICE_PLATFORM/$NICE_ARCH"
	cp -av binaries/*.rpm $destdir
	cp -av binaries/*.deb $destdir
	cp -av binaries/*.pkg.tar.xz $destdir
	cp -av binaries/*.tgz $destdir
	rm -rf binaries
		
done < /tmp/results.$$
rm /tmp/results.$$
IFS=$OLDIFS

exit
