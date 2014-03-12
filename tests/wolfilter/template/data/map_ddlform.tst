#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testcmd="-c wolframe.conf employee_assignment_print"	# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE ddlcompiler_simpleform`
	module `cmd/MODULE normalize_number`
	module `cmd/MODULE normalize_string`
	module `cmd/MODULE datatype_bcdnumber`
}
Processor
{
	program `cmd/PROGRAM employee_assignment_print.sfrm`
	program `cmd/PROGRAM simpleform.wnmp`
}"
. ./output_tst_all.sh

