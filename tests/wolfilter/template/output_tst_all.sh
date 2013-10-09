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
# - dumpout		(optional) file to dump to expected output too
# - testdata		(optional) additionaly defined test data
# - inputfilter		(optional) explicitely defined inputfilter
# - outputfilter	(optional) explicitely defined outputfilter
#
TESTCMD="$testcmd"
TESTNAME="$testname"
CSETLIST=$csetlist
INPUTFILTER="$inputfilter"
OUTPUTFILTER="$outputfilter"
if [ `echo $testcmd | grep -c -- '--config'` = 0 ]; then
	if [ x"$csetlist" = x ]; then
		csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4LE UCS-4BE"
	fi
	. ./output_tst_textwolf.sh
	csetlist="$CSETLIST"
	if [ x"$csetlist" = x ]; then
		csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
	fi
	. ./output_tst_libxml2.sh
	csetlist="$CSETLIST"
	if [ x"$csetlist" = x ]; then
		csetlist="UTF-8 UTF-16LE UTF-16BE UCS-4LE UCS-4BE"
	fi
	. ./output_tst_json.sh
	csetlist="$CSETLIST"
else
	if [ x"$csetlist" = x ]; then
		csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
	fi
	. ./output_tst_nofilter.sh
	csetlist="$CSETLIST"
fi
testcmd="$TESTCMD"
testname="$TESTNAME"
inputfilter="$INPUTFILTER"
outputfilter="$OUTPUTFILTER"
