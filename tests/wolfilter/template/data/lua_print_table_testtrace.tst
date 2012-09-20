#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//' | sed 's/_testtrace//'`.lua
formname="invoice.simpleform"
opt=""
layout="f=invoice.simplepdf,n=print_invoice,t=tracepdf"
modpath="../../src/modules/ddlcompiler/"		# DDL compiler module directory relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# DDL compiler module to load
opt="$opt --module $mod"
modpath="../../src/modules/prnt/"			# printer module directory relative from tests/temp
mod="$modpath/testPdfPrinter/mod_test_pdf_printer"	# printer module to load
opt="$opt --module $mod"
modpath="../../src/modules/filter/"			# filter module directory relative from tests/temp
mod="$modpath/blob/mod_filter_blob"			# blob filter module for output to load
opt="$opt --module $mod"
opt="$opt --printlayout $layout"			# layout for printing invoice
opt="$opt --form $formname"				# form for invoice
opt="$opt --script $luascript"				# script to execute
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript $formname invoice.simplepdf"	# list of scripts of the test
docin=invoice_example					# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
