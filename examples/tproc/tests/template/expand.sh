#!/bin/sh

for ff in `find data -type f`
	echo Expanding template $ff
	do $ff 
done

