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
# - Script called in the context of a test script (as ". output_tst_all.sh") with the following variables set
#
# VARIABLES
# - testname		name of the test
# - testcmd		command to execute by the test
# - testscripts		list of scripts of the test
# - docin		input document name
# - docout		output document name
#
csetlist_textwolf="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4LE UCS-4BE"
csetlist_libxml2="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
TESTNAME="$testname"

filter="xml:textwolf"
for cset in $csetlist_textwolf
do
	inputfilter="$filter"
	outputfilter="$filter"
	. ./output_tst.sh
	echo "echo executing test $testname $cset" >> ../../testWolfilter.sh
	echo "cat wolfilter/template/doc/$docin.UTF-8.xml | sed 's/UTF-8/$cset/' | recode UTF-8..$cset | ../wtest/cleanInput BOM EOLN | ../src/wolfilter `echo --input-filter $inputfilter --output-filter $outputfilter $testcmd | sed 's@--form @--form wolfilter/scripts/@' | sed 's@--script @--script wolfilter/scripts/@'` > temp/$docout.$cset.xml" >> ../../testWolfilter.sh
done

filter="xml:libxml2"
testname="$TESTNAME""_libxml2"
for cset in $csetlist_libxml2
do
	inputfilter="$filter"
	outputfilter="$filter"
	. ./output_tst.sh
	echo "echo executing test $testname $cset" >> ../../testWolfilter.sh
	echo "cat wolfilter/template/doc/$docin.UTF-8.xml | sed 's/UTF-8/$cset/' | recode UTF-8..$cset | ../wtest/cleanInput BOM EOLN | ../src/wolfilter `echo --input-filter $inputfilter --output-filter $outputfilter $testcmd | sed 's@--form @--form wolfilter/scripts/@' | sed 's@--script @--script wolfilter/scripts/@'` > temp/$docout.$cset.xml" >> ../../testWolfilter.sh
done
