#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=schema_select_task_by_id				# input document name
docout=output_schema_select_task_by_id			# output document name
dumpout="program/schema_select_task_by_id.dbdump.txt"	# resource dump to add to expected test output
testcmd="-c wolframe.conf schema_select_task_by_id"	# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_db_sqlite3test`
	module `cmd/MODULE mod_normalize_number`
	module `cmd/MODULE mod_normalize_string`
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_ddlcompiler_simpleform`
	module `cmd/MODULE mod_datatype_bcdnumber`
}
Database
{
	SQliteTest
	{
		identifier testdb
		file test.db
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program `cmd/PROGRAM schema_select_task_by_id_UTF16BE.tdl`
	program `cmd/PROGRAM schema_select_task_by_id.sfrm`
	program `cmd/PROGRAM simpleform.wnmp`

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
COMMAND schema_select_task_by_id CALL test_transaction RETURN STANDALONE doc;
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.tdl | recode UTF-8..UTF16BE`
**outputfile:DBDUMP"
. ./output_tst_all.sh

