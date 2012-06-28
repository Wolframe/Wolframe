#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd='- xml:textwolf'				# command to execute by the test
testscripts=""						# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$docin						# output document name
. ./output_tst_all.sh
