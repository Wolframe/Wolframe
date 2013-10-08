#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
opt="$opt --module `cmd/MODULE command_directmap`"
opt="$opt --module `cmd/MODULE ddlcompiler_simpleform`"
opt="$opt --module `cmd/MODULE normalize_number`"
opt="$opt --module `cmd/MODULE normalize_string`"
opt="$opt --cmdprogram=test.dmap"
opt="$opt --program employee_assignment_print.sfrm"
opt="$opt --program simpleform.wnmp"
opt="$opt --program echo_invoice_table.wnmp"

testcmd="$opt employee_assignment_print"
docin=invoice_example
docout=invoice_example

testdata="
`cmd/INCFILE employee_assignment_print.sfrm`
`cmd/INCFILE simpleform.wnmp`
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction);"
. ./output_tst_all.sh
