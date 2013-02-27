#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
formname="employee_assignment_print.simpleform"
ddltypeprg="simpleform.normalize"
opt=""
modpath="../../src/modules/cmdbind/directmap"		# module directory relative from tests/temp
opt="$opt --module $modpath/mod_command_directmap"
modpath="../../src/modules/ddlcompiler/"		# module directory for DDL compilerds relative from tests/temp
mod="$modpath/simpleform/mod_ddlcompiler_simpleform"	# module to load
opt="$opt --module $mod"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/number/mod_normalize_number"		# module to load
opt="$opt --module $mod"
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
modpath="../wolfilter/modules/database"			# module directory relative from tests/temp
opt="$opt --module $modpath/testtrace/mod_db_testtrace"
opt="$opt --database 'identifier=testdb,outfile=DBOUT,file=DBRES'"
opt="$opt --program=DBIN.tdl"
opt="$opt --program=test.directmap"
opt="$opt --program $formname"				# form for invoice
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
testcmd="$opt run"					# command to execute by the test
testscripts="$formname"					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=map_transaction					# output document name
testdata="
**file:$ddltypeprg
`cat program/$ddltypeprg`
**file: test.directmap
run = test_transaction( xml :employee_assignment_print);
**file: DBRES
#id task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#id task start end#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO task FOREACH //task DO run( title);
END"
. ./output_tst_all.sh