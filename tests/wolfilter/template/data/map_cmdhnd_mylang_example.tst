#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=cdcatalog.in					# input document name
docout=cdcatalog.out					# output document name
testcmd="-c wolframe.conf getCDCatalog"		# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_directmap`
	module `cmd/MODULE ddlcompiler_simpleform`
	module `cmd/MODULE normalize_number`
	module `cmd/MODULE normalize_string`
	module `cmd/MODULE mod_command_mylang`
}
Processor
{
	program `cmd/PROGRAM example.mlg`
	program `cmd/PROGRAM example.sfrm`
	program `cmd/PROGRAM example.wnmp`
	
	cmdhandler
	{
		directmap
		{
			program `cmd/PROGRAM example.dmap`
			filter #FILTER#
		}
	}
}"
csetlist="UTF-8"
. ./output_tst_all.sh

