#!/bin/sh
testname=`basename $0 ".tst"`		# name of the test
docin=invoice_example
docout=invoice_example_out
testcmd="-c wolframe.conf echoInvoice"	# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_lua`
	module `cmd/MODULE command_directmap`
	module `cmd/MODULE ddlcompiler_simpleform`
	module `cmd/MODULE normalize_number`
	module `cmd/MODULE normalize_string`
}
Processor
{
	program `cmd/PROGRAM invoice.sfrm`
	program `cmd/PROGRAM simpleform.wnmp`
	program `cmd/PROGRAM echo_invoice_table.lua`
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter #FILTER#
		}
	}
}
**file: test.dmap
COMMAND(echo Invoice) CALL(echo_invoice_table) RETURN Invoice;"
. ./output_tst_all.sh

