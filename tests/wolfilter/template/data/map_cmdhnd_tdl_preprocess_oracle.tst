#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd="$opt AllDataRequest"				# command to execute by the test
docin=tdl_preprocess.in					# input document name
docout=tdl_preprocess.out				# output document name
dumpout="program/tdl_preprocess/oracle.dbdump.txt"	# resource dump to add to expected test output
testcmd="-c wolframe.conf AllDataRequest"		# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_db_oracletest`
	module `cmd/MODULE mod_command_lua`
	module `cmd/MODULE mod_normalize_number`
	module `cmd/MODULE mod_normalize_string`
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_ddlcompiler_simpleform`
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
	program preprocess.lua
	program preprocess.sfrm
	program preprocess.tdl
	program preprocess.wnmp

	cmdhandler
	{
		directmap
		{
			program preprocess.dmap
			filter #FILTER#
		}
	}
}
**file:DBDATA
`cat program/tdl_preprocess/schema_oracle.sql`
**file:preprocess.lua
`cat program/tdl_preprocess/preprocess.lua`
**file:preprocess.dmap
`cat program/tdl_preprocess/preprocess.dmap`
**file:preprocess.sfrm
`cat program/tdl_preprocess/preprocess.sfrm`
**file:preprocess.tdl
`cat program/tdl_preprocess/preprocess-oracle.tdl`
**file:preprocess.wnmp
`cat program/tdl_preprocess/preprocess.wnmp`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
