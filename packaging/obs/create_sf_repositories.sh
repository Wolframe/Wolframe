#!/bin/sh

if test $# != 1; then
	echo "Usage: packaging/obs/create_sf_repositories.sh <release>" 1>&2
	echo "where release is CentOS-6, CentOS-5, RHEL-6, RHEL-5" 1>&2
	echo "                 Fedora-20, Fedora-19" 1>&2
	echo "                 ArchLinux" 1>&2
	exit 1
fi
	
SOURCEFORGE_ROOT=/mnt/sf

BASE=$PWD

DISTRO=$1

if test $DISTRO = "Fedora-20"; then

	REPODIR=/mnt/sf/repositories/Fedora-20
	
	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/Fedora_20/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/Fedora_20/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|Fedora_20|Fedora-20|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/*primary.xml.gz $REPODIR/repodata
	cp repodata/*filelists.xml.gz $REPODIR/repodata
	cp repodata/*other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i686
	cp -v i686/*.rpm $REPODIR/i686/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "Fedora-19"; then

	REPODIR=/mnt/sf/repositories/Fedora-19
	
	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/Fedora_19/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/Fedora_19/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|Fedora_19|Fedora-19|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/*primary.xml.gz $REPODIR/repodata
	cp repodata/*filelists.xml.gz $REPODIR/repodata
	cp repodata/*other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i686
	cp -v i686/*.rpm $REPODIR/i686/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "CentOS-6"; then

	REPODIR=/mnt/sf/repositories/CentOS-6

	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/CentOS_CentOS-6/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/CentOS_CentOS-6/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|CentOS_CentOS-6|CentOS-6|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/primary.xml.gz $REPODIR/repodata
	cp repodata/filelists.xml.gz $REPODIR/repodata
	cp repodata/other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i686
	cp -v i686/*.rpm $REPODIR/i686/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "CentOS-5"; then

	REPODIR=/mnt/sf/repositories/CentOS-5

	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/CentOS_CentOS-5/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/CentOS_CentOS-5/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|CentOS_CentOS-5|CentOS-5|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/primary.xml.gz $REPODIR/repodata
	cp repodata/filelists.xml.gz $REPODIR/repodata
	cp repodata/other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i386
	cp -v i386/*.rpm $REPODIR/i386/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "RHEL-6"; then

	REPODIR=/mnt/sf/repositories/RHEL-6

	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/RedHat_RHEL-6/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/RedHat_RHEL-6/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|RedHat_RHEL-6|RHEL-6|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/primary.xml.gz $REPODIR/repodata
	cp repodata/filelists.xml.gz $REPODIR/repodata
	cp repodata/other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i686
	cp -v i686/*.rpm $REPODIR/i686/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "RHEL-5"; then

	REPODIR=/mnt/sf/repositories/RHEL-5

	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/RedHat_RHEL-5/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/RedHat_RHEL-5/

	sed 's|http://download.opensuse.org/repositories/home:/wolframe_user/|http://sourceforge.net/projects/wolframe/files/repositories/|g' \
		home:wolframe_user.repo | \
	sed 's|\[home_wolframe_user\]|[Wolframe]|g' | \
	sed 's|wolframe_user.s Home Project|Wolframe Project|g' | \
	sed 's|RedHat_RHEL-5|RHEL-5|g' \
		> $REPODIR/wolframe.repo

	mkdir -p $REPODIR/repodata

	cp repodata/repomd.xml.key $REPODIR/repodata
	cp repodata/repomd.xml $REPODIR/repodata
	cp repodata/primary.xml.gz $REPODIR/repodata
	cp repodata/filelists.xml.gz $REPODIR/repodata
	cp repodata/other.xml.gz $REPODIR/repodata

	mkdir -p $REPODIR/x86_64
	cp -v x86_64/*.rpm $REPODIR/x86_64/.
	mkdir -p $REPODIR/i386
	cp -v i386/*.rpm $REPODIR/i386/.
	mkdir -p $REPODIR/src
	cp -v src/*.src.rpm $REPODIR/src/.
	
	cd $BASE
	rm -rf download.opensuse.org
fi

if test $DISTRO = "ArchLinux"; then

	REPODIR=/mnt/sf/repositories/ArchLinux

	rm -vrf $REPODIR
	mkdir -p $REPODIR

	wget --no-parent -m http://download.opensuse.org/repositories/home:/wolframe_user/Arch_Extra/
	cd $BASE/download.opensuse.org/repositories/home:/wolframe_user/Arch_Extra/

	mkdir -p $REPODIR/i686
	mkdir -p $REPODIR/x86_64

	cp i686/home_wolframe_user_Arch_Extra.db $REPODIR/i686/wolframe.db
	cp i686/home_wolframe_user_Arch_Extra.db.sig $REPODIR/i686/wolframe.db.sig
	cp x86_64/home_wolframe_user_Arch_Extra.db $REPODIR/x86_64/wolframe.db
	cp x86_64/home_wolframe_user_Arch_Extra.db.sig $REPODIR/x86_64/wolframe.db.sig

	cp -v i686/*.pkg.tar.xz $REPODIR/i686/
	cp -v x86_64/*.pkg.tar.xz $REPODIR/x86_64/

	cd $BASE
	rm -rf download.opensuse.org
fi
