#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
formname_out="$testname.simpleform"			# output form
opt="$opt --program $formname_out"
formname_in="employee_assignment_print.simpleform"	# input form
opt="$opt --program $formname_in"
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilers relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
ddltypeprg="simpleform.normalize"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/testtrace/mod_db_testtrace"
opt="$opt --database 'identifier=testdb,outfile=DBOUT,file=DBRES'"
opt="$opt --program=DBIN.tdl"
opt="$opt --cmdprogram=test.directmap"
testcmd="$opt run"					# command to execute by the test
testscripts="$formname_in $formname_out"		# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=map_cmdhnd_transaction_outputform		# output document name
testdata="
**file: test.directmap
run = test_transaction( xml :employee_assignment_print) :$testname;
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: DBRES
#id task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#id task start end#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO run( title);
END"
. ./output_tst_all.sh
