#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/testtrace/mod_db_testtrace"
opt="$opt --database 'identifier=testdb,outfile=DBOUT,file=DBRES'"
opt="$opt --program=DBIN"
luascript=`echo $testname | sed 's/lua_//'`.lua
opt="$opt --script $luascript"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file: DBRES
#name#job1#job2#job3#job4
**file:DBIN
TRANSACTION test_transaction
BEGIN
	INTO doc/title WITH //task DO run( title);
END"
. ./output_tst_all.sh
