#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
formname="typed_invoice.simpleform"
opt=""
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilerds relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/locale/mod_normalize_locale"		# module to load
opt="$opt --module $mod"
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"

# form data type definitions:
opt="$opt --normalize 'int number integer(10)'"
opt="$opt --normalize 'f_int number trim integer(10)'"
opt="$opt --normalize 'uint number unsigned(10)'"
opt="$opt --normalize 'f_uint number trim unsigned(10)'"
opt="$opt --normalize 'float number float(10,10)'"
opt="$opt --normalize 'f_float number trim float(10,10)'"
opt="$opt --normalize 'text_ascii_de localeconv latinword,ascii_de'"
opt="$opt --normalize 'text_ascii_eu localeconv latinword,ascii_eu'"

opt="$opt --program $formname"				# form for invoice
testcmd="$opt typed_invoice"				# command to execute by the test
testscripts="$formname"					# list of scripts of the test
docin=charconv_example					# input document name
docout=$testname					# output document name
codepage="1"
. ./output_tst_all_isolatin.sh

