#!/bin/sh
testname=`basename $0 ".tst"`			# name of the test
docin=charconv_example				# input document name
docout=$testname				# output document name
testcmd="-c wolframe.conf typed_invoice"	# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE ddlcompiler_simpleform`
	module `cmd/MODULE normalize_number`
	module `cmd/MODULE normalize_locale`
	module `cmd/MODULE normalize_string`
}
Processor
{
	program `cmd/PROGRAM typed_invoice.sfrm`
	program `cmd/PROGRAM simpleform_complex.wnmp`
}"
codepage="1"
. ./output_tst_all_isolatin.sh

