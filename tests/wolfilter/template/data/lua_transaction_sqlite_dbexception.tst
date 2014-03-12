#!/bin/sh
testname=`basename $0 ".tst"`			# name of the test
luascript=`echo $testname | sed 's/lua_//' | sed 's/_sqlite//'`.lua
docname=`echo $testname | sed 's/_sqlite//'`
docin=$docname.in				# input document name
dumpout="program/dbexception/sqlite_dbdump.txt"	# resource dump to add to expected test output
expecterror="error in transaction 'insertCustomer':*Customers must have a unique name."
testcmd="-c wolframe.conf run"			# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
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
`cat program/dbexception/dbexception_schema_sqlite.sql`
**file:DBPRG.tdl
`cat program/dbexception/dbexception.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
