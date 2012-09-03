#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//' | sed 's/_testtrace//'`.lua
formname="invoice.simpleform"
layout="print_invoice.tracepdf:invoice.simplepdf"
opt="--printlayout $layout"				# layout for printing invoice
opt="$opt --form $formname"				# form for invoice
opt="$opt --script $luascript"				# script to execute
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript $formname"			# list of scripts of the test
docin=invoice_example					# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
