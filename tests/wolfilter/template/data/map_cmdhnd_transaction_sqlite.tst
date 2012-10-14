#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/sqlite3/mod_db_sqlite3test"
opt="$opt --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA'"
opt="$opt --program=DBPRG"
opt="$opt --directmap 'f=xml,c=test_transaction,n=run'"
testcmd="$opt run"					# command to execute by the test
docin=employee_assignment_print				# input document name
docout=map_transaction					# output document name
disabled=1
testdata="
**file: DBDATA
`cat db/schema_select_task_by_id.sql`
**file:DBPRG
`cat db/schema_select_task_by_id.prg`
**outputfile:DBDUMP"
. ./output_tst_all.sh
