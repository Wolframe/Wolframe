#!/bin/sh

# This file is not part of Wolframe. It is used to generate test documents out of a description.
# Do not execute this unless you know what you do. Executing this script might invalidate the
# test collection. 

for ff in `find data -type f -name "*.tst"`; do
	echo Expanding template $ff
	./$ff 
done

