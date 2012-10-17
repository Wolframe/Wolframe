#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
ddltypeprg="simpleform.normalize"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/sqlite3/mod_db_sqlite3test"
opt="$opt --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA'"
opt="$opt --program=DBPRG.tdl"
opt="$opt --directmap 'f=xml,c=test_transaction,n=run'"
testcmd="$opt run"					# command to execute by the test
docin=employee_assignment_print				# input document name
docout=map_transaction					# output document name
disabled=1
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.prg`
**outputfile:DBDUMP"
. ./output_tst_all.sh
