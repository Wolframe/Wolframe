#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
modmain="../wolfilter/modules/"
modpath="$modmain/employee_assignment_convert/"		# module directory relative from tests/temp
mod="$modpath/mod_employee_assignment_convert"		# module to load
luascript=`echo $testname | sed 's/lua_//'`.lua		# script to execute
testcmd="--module $mod --script $luascript run"		# command to execute by the test
testscripts=$luascript					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
