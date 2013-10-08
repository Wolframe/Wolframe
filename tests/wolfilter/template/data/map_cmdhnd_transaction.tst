#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
opt="$opt --module `cmd/MODULE command_directmap`"
opt="$opt --module `cmd/MODULE ddlcompiler_simpleform`"
opt="$opt --module `cmd/MODULE normalize_number`"
opt="$opt --module `cmd/MODULE normalize_string`"
opt="$opt --module `cmd/MODULE db_testtrace`"
opt="$opt --program `cmd/PROGRAM employee_assignment_print.sfrm`"
opt="$opt --program `cmd/PROGRAM simpleform.wnmp`"
opt="$opt --database 'identifier=testdb,outfile=DBOUT,file=DBRES'"
opt="$opt --program=DBIN.tdl"
opt="$opt --cmdprogram=test.dmap"
testcmd="$opt employee_assignment_print"

docin=employee_assignment_print
docout=map_transaction

testdata="
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction);
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
