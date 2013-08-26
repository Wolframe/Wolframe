#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
formname="employee_assignment_print.sfrm"
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilers relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
ddltypeprg="simpleform_range.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --program $formname"
opt="$opt --cmdprogram $luascript"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript $formname"			# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`"
. ./output_tst_all.sh
