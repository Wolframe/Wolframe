#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
modmain="../wolfilter/modules/"
modpath="$modmain/employee_assignment_convert/"		# module directory relative from tests/temp
mod="$modpath/mod_employee_assignment_convert.so"	# module to load
testcmd="--module $mod employee_assignment_convert"	# command to execute by the test
testscripts=""						# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
