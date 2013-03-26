#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//' | sed 's/_testtrace//'`.lua
formname="invoice.simpleform"
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
modpath="../../src/modules/ddlcompiler/"		# DDL compiler module directory relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# DDL compiler module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
modpath="../../src/modules/prnt/"			# printer module directory relative from tests/temp
mod="$modpath/testPdfPrinter/mod_test_pdf_printer"	# printer module to load
opt="$opt --module $mod"
modpath="../../src/modules/filter/"			# filter module directory relative from tests/temp
mod="$modpath/blob/mod_filter_blob"			# blob filter module for output to load
opt="$opt --module $mod"
ddltypeprg="simpleform_range.normalize"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --program invoice.simplepdf"			# layout for printing invoice
opt="$opt --program $formname"				# form for invoice
opt="$opt --cmdprogram $luascript"			# script to execute
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript $formname invoice.simplepdf"	# list of scripts of the test
docin=invoice_example					# input document name
docout=$testname					# output document name
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`"
. ./output_tst_all.sh
