#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
modmain="../wolfilter/modules/"
modpath="$modmain/employee_assignment_convert/"		# module directory relative from tests/temp
mod="$modpath/mod_employee_assignment_convert"		# module to load
opt="$opt --module $mod"
luascript=`echo $testname | sed 's/lua_//'`.lua		# script to execute
testcmd="$opt --cmdprogram $luascript run"		# command to execute by the test
testscripts=$luascript					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
