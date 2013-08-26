#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
formname="employee_assignment_print.sfrm"
opt=""
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilerds relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
ddltypeprg="simpleform.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --program $formname"				# form for invoice
testcmd="$opt employee_assignment_print"		# command to execute by the test
testscripts="$formname"					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`"
. ./output_tst_all.sh
