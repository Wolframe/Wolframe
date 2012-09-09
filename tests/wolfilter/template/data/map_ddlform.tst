#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
formname="employee_assignment_print.simpleform"
modpath="../../src/modules/ddlcompiler/"		# module directory relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="--module $mod"
opt="$opt --form $formname"				# form for invoice
testcmd="$opt employee_assignment_print"		# command to execute by the test
testscripts="$formname"					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
