#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/testtrace/mod_db_testtrace"
opt="$opt --database 'identifier=testdb,outfile=DBOUT,file=DBRES'"
opt="$opt --program=DBIN.tdl"
luascript=`echo $testname | sed 's/lua_//'`.lua		# script to execute
opt="$opt --cmdprogram $luascript"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file: DBRES
#name#job1
#name#job2
#name#job3
#name#job4
**file:DBIN.tdl
TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO title FOREACH //task DO run( title);
END"
. ./output_tst_all.sh
