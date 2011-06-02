#!/bin/sh

if test ! -f repo.cache; then
	osc repos > repo.cache
fi

for r in `cat repo.cache | tr -s ' ' '@'`; do
	repo=`echo $r | cut -f 1 -d@`
	arch=`echo $r | cut -f 2 -d@`
	echo "Getting packages for $repo, $arch"
	osc getbinaries $repo $arch
	
	echo "Getting buildlog for $repo, $arch"
	osc buildlog $repo $arch > buildlog-$repo-$arch.log	
done
