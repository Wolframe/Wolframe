#!/bin/sh
testname=`basename $0 ".tst"`						# name of the test
formname="employee_assignment_print.simpleform"
testcmd="--form $formname employee_assignment_print xml:textwolf"	# command to execute by the test
testscripts="$formname"							# list of scripts of the test
docin=employee_assignment_print						# input document name
docout=$testname							# output document name
. ./output_tst_all.sh
