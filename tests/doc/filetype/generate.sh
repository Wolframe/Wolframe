#!/bin/sh

# THIS FILE IS NOT PART OF WOLFRAME !
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.
#/

for ff in test-with-header test-without-header
do
	for ext in unknown xml
	do
		for cset in UCS-2BE UCS-2BE UCS-4BE UCS-4BE
		do
			echo "create $ff.$cset.$ext"
			cat $ff.UTF-8.$ext | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN > $ff.$cset.$ext
			echo "create $ff.BOM.$cset.$ext"
			cat $ff.UTF-8.$ext | sed "s/UTF-8/$cset/" | recode UTF-8..$cset > $ff.BOM.$cset.$ext
		done
	done
done
