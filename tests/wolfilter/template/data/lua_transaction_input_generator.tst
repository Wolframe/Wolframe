#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua		# script to execute
testcmd="-c wolframe.conf test_transaction"		# command to execute by the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	program		DBIN.tdl
	program		script.lua
	database	testdb
}
**file:script.lua
`cat ../scripts/$luascript`
**file: DBRES
#name#job1
#name#job2
#name#job3
#name#job4
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO title FOREACH //task DO SELECT run( title);
END"
. ./output_tst_all.sh

