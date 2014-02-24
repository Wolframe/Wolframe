#!/bin/sh
testname=`basename $0 ".tst"`			# name of the test
luascript=`echo $testname | sed 's/lua_//' | sed 's/_oracle//'`.lua
docname=`echo $testname | sed 's/_oracle//'`
docin=$docname.in				# input document name
docout=$docname.out				# output document name
dumpout="program/$testname.dbdump.txt"		# resource dump to add to expected test output
testcmd="-c wolframe.conf run"			# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/oracle/mod_db_oracletest
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
}
Database
{
	OracleTest
	{
		identifier testdb
		host andreasbaumann.dyndns.org
		port 1521
		database orcl
		user wolfusr
		password wolfpwd
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program normalize.wnmp
	program DBPRG.tdl
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:normalize.wnmp
`cat program/simpleform.wnmp`
**file:script.lua
`cat ../scripts/$luascript`
**file: DBDATA
`cat program/first/tree_schema_oracle.sql`
**file:DBPRG.tdl
`cat program/first/tree_transactions.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
