#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=audit.in						# input document name (no output document)
dumpout="program/audit/aamap_oracle.dbdump.txt"		# resource dump to add to expected test output
testcmd="-c wolframe.conf InsertPerson"			# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_db_oracletest`
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_command_aamap`
	module `cmd/MODULE mod_audit`
}
Database
{
	OracleTest
	{
		identifier testdb
		host andreasbaumann.dyndns.org
		database orcl
		port 1521
		user wolfusr
		password wolfpwd
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program audit.tdl
	program audit.aamap

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
`cat program/audit/schema_oracle.sql`
**file:audit.dmap
`cat program/audit/audit.dmap`
**file:audit.aamap
`cat program/audit/audit.aamap`
**file:audit.tdl
`cat program/audit/audit.tdl`
**outputfile:DBDUMP
**outputfile:audit.log"
. ./output_tst_all.sh

