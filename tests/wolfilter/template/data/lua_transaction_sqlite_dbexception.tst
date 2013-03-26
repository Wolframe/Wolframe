#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
luascript=`echo $testname | sed 's/lua_//' | sed 's/_sqlite//'`.lua
opt="$opt --cmdprogram=$luascript"
ddltypeprg="simpleform.normalize"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/sqlite3/mod_db_sqlite3test"
opt="$opt --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA'"
opt="$opt --program=DBPRG.tdl"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docname=`echo $testname | sed 's/_sqlite//'`
docin=$docname.in					# input document name
# docout=$docname.out					# output document name
dumpout="program/dbexception/sqlite_dbdump.txt"		# resource dump to add to expected test output
expecterror="error in transaction insertCustomer:column name is not unique . Customers must have a unique name."
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/dbexception/dbexception_schema_sqlite.sql`
**file:DBPRG.tdl
`cat program/dbexception/dbexception.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
