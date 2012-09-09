#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd="echo_transaction"				# command to execute by the test
testscripts="$luascript $formname"			# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=map_ddlform					# output document name
. ./output_tst_all.sh
