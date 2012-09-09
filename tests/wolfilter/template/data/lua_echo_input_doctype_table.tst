#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
formname="employee_assignment_print.simpleform"
modpath="../../src/modules/ddlcompiler/"		# module directory relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="--module $mod"
opt="$opt --form $formname"
opt="$opt --script $luascript"
testcmd="$opt run"					# command to execute by the test
docin=employee_assignment_doctype_2			# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
