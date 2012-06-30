#!/bin/sh

#
# THIS FILE IS NOT PART OF WOLFRAME !
#
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.
#
# DESCRIPTION
# - Script called in the context of a test script (as ". output_tst.sh") with the following variables set
#
# VARIABLES
# - testname		name of the test
# - testcmd		command to execute by the test
# - cset		character set encoding name
# - testscripts		list of scripts of the test
# - docin		input document name
# - docout		output document name
# - inputfilter		input filter name
# - outputfilter	output filter name
#
output="../data/$testname.$cset.tst"
rm -f $output
echo "Writing test file $output"

echo '**' >> $output
echo '**input' >> $output
cat doc/$docin.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo '**config' >> $output
echo "--input-filter $inputfilter --output-filter $outputfilter $testcmd" >> $output
for script in $testscripts
do
	echo "**file: $script" >> $output
	cat ../scripts/$script >> $output
done
echo '**output' >> $output
cat doc/$docout.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset >> $output
echo '**end' >> $output
