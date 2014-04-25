#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=audit.in						# input document name (no output document)
dumpout="program/audit/sqlite.dbdump.txt"		# resource dump to add to expected test output
testcmd="-c wolframe.conf InsertPerson"			# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_db_sqlite3test`
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_audit`
}
Database
{
	SQliteTest
	{
		identifier testdb
		file test.db
		connections 3
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program audit.tdl

	cmdhandler
	{
		directmap
		{
			program audit.dmap
			filter #FILTER#
		}
	}
}
**file:DBDATA
`cat program/audit/schema_sqlite.sql`
**file:audit.dmap
`cat program/audit/audit.dmap`
**file:audit.tdl
`cat program/audit/audit.tdl`
**outputfile:DBDUMP
**outputfile:audit.log"
. ./output_tst_all.sh

