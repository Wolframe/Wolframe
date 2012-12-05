#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd='- '						# command to execute by the test
testscripts=""						# list of scripts of the test
docin=$testname.in					# input document name
docout=$testname.out					# output document name
. ./output_tst_all.sh
