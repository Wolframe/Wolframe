#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
formname="typed_invoice.sfrm"
opt=""
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilerds relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/locale/mod_normalize_locale"		# module to load
opt="$opt --module $mod"
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
ddltypeprg="simpleform_complex.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --program $formname"				# form for invoice
testcmd="$opt typed_invoice"				# command to execute by the test
testscripts="$formname"					# list of scripts of the test
docin=charconv_example					# input document name
docout=$testname					# output document name
codepage="1"
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`"
. ./output_tst_all_isolatin.sh
