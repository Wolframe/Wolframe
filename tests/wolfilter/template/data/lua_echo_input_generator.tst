#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
testcmd="--script $luascript run"			# command to execute by the test
docin=employee_assignment_print				# input document name
docout=$docin						# output document name
. ./output_tst_all.sh
