#!/bin/sh

for ff in `find data -type f`; do
	echo Expanding template $ff
	./$ff 
done

