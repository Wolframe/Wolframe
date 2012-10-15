#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
formname="employee_assignment_print.simpleform"
opt=""
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilers relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
opt="$opt --normalize 'int number integer(10)'"		# default form datatype for singed integer number
opt="$opt --normalize 'uint number unsigned(10)'"	# default form datatype for unsinged integer number
opt="$opt --normalize 'float number float(10,10)'"	# default form datatype for floating point number
opt="$opt --program $formname"
opt="$opt --program $luascript"
testcmd="$opt run"					# command to execute by the test
docin=employee_assignment_doctype_2			# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
