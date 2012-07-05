#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
formname="employee_assignment_print.simpleform"
testcmd="--form $formname --script $luascript run"	# command to execute by the test
docin=employee_assignment_doctype			# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
