#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
opt="$opt --module `cmd/MODULE command_directmap`"
opt="$opt --module `cmd/MODULE command_lua`"
opt="$opt --module `cmd/MODULE ddlcompiler_simpleform`"
opt="$opt --module `cmd/MODULE normalize_number`"
opt="$opt --module `cmd/MODULE normalize_string`"
opt="$opt --cmdprogram=test.dmap"
opt="$opt --program `cmd/PROGRAM invoice.sfrm`"
opt="$opt --program `cmd/PROGRAM simpleform.wnmp`"
opt="$opt --program `cmd/PROGRAM echo_invoice_table.lua`"

testcmd="$opt echoInvoice"
docin=invoice_example
docout=invoice_example_out

testdata="
**file: test.dmap
COMMAND(echo Invoice) CALL(echo_invoice_table) RETURN Invoice;"
. ./output_tst_all.sh
