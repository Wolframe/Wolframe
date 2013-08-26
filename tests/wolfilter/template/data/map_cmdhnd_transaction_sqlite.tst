#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
formname="schema_select_task_by_id.sfrm"
opt="$opt --program $formname"
ddltypeprg="simpleform.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilers relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
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
opt="$opt --cmdprogram=test.dmap"
testscripts="$formname"					# list of scripts of the test
testcmd="$opt run"					# command to execute by the test
docin=schema_select_task_by_id				# input document name
docout=output_schema_select_task_by_id			# output document name
dumpout="program/schema_select_task_by_id.dbdump.txt"	# resource dump to add to expected test output
testdata="
**file: test.dmap
run = test_transaction( xml :schema_select_task_by_id);
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.tdl`
**outputfile:DBDUMP"
. ./output_tst_all.sh
