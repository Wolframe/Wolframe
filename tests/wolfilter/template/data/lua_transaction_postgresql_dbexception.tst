#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
luascript=`echo $testname | sed 's/lua_//' | sed 's/_postgresql//'`.lua
opt="$opt --program=$luascript"
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
opt="$opt --module $modpath/postgresql/mod_db_postgresqltest"
opt="$opt --database 'identifier=testdb,host=localhost,port=5432,database=wolframe,user=wolfusr,password=wolfpwd,dumpfile=DBDUMP,inputfile=DBDATA'"
opt="$opt --program=DBPRG.tdl"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docname=`echo $testname | sed 's/_postgresql//'`
docin=$docname.in					# input document name
# docout=$docname.out					# output document name
dumpout="program/dbexception/postgresql_dbdump.txt"	# resource dump to add to expected test output
expecterror="error in transaction insertCustomer:  Key (name)=(Hugo) already exists. "
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/dbexception/dbexception_schema_postgresql.sql`
**file:DBPRG.tdl
`cat program/dbexception/dbexception.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
