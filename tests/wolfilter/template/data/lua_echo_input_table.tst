#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
testcmd="--program $luascript run"			# command to execute by the test
docin=employee_assignment_print_2			# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
