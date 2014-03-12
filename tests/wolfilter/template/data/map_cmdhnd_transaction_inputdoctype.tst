#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=employee_assignment_doctype			# input to process
docout=map_transaction					# output expected
testcmd="-c wolframe.conf employee_assignment_print"	# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_db_testtrace`
	module `cmd/MODULE mod_normalize_number`
	module `cmd/MODULE mod_normalize_string`
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_ddlcompiler_simpleform`
	module `cmd/MODULE mod_datatype_bcdnumber`
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	database testdb
	program DBIN.tdl
	program `cmd/PROGRAM employee_assignment_print.sfrm`
	program `cmd/PROGRAM simpleform_range_trim.wnmp`

	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter #FILTER#
		}
	}
}
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction) RETURN STANDALONE doc;
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

