#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=print_table_libxml2.lua
formname="invoice.sfrm"
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
modpath="../../tests/wolfilter/modules/prnt/"		# printer module directory relative from tests/temp
mod="$modpath/fakepdfprint/mod_print_testpdf"		# printer module to load
opt="$opt --module $mod"
modpath="../../src/modules/filter/"			# filter module directory relative from tests/temp
mod="$modpath/blob/mod_filter_blob"			# blob filter module for output to load
opt="$opt --module $mod"
ddltypeprg="simpleform_range.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --program invoice.sprn"			# layout for printing invoice
opt="$opt --program $formname"				# form for invoice
opt="$opt --cmdprogram $luascript"			# script to execute
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript $formname invoice.sprn"		# list of scripts of the test
docin=invoice_example					# input document name
docout=lua_print_table_testtrace			# output document name
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`"
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
. ./output_tst_libxml2.sh
