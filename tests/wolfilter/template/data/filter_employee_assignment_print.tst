#!/bin/sh
testname=`basename $0 ".tst"`			# name of the test
testcmd='- '					# command to execute by the test
testscripts=""					# list of scripts of the test
docin=`echo $testname | sed 's/filter_//'`	# input document name
docout=$docin					# output document name
. ./output_tst_all.sh
