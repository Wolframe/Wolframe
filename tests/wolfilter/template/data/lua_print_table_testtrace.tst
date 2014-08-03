#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=print_table_libxml2.lua
testcmd="-c wolframe.conf run"				# command to execute by the test
docin=invoice_example_envelop				# input document name
docout=lua_print_table_testtrace			# output document name
testdata="
**requires:LIBHPDF
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
	module ../../src/modules/filter/blob/mod_filter_blob
	module ../wolfilter/modules/prnt/fakepdfprint/mod_print_testpdf
}
Processor
{
	program normalize.wnmp
	program print.sprn
	program form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
			filter #FILTER#
		}
	}
}
**file:form.sfrm
`cat ../scripts/invoice.sfrm`
**file:normalize.wnmp
`cat program/simpleform_range.wnmp`
**file:print.sprn
`cat ../scripts/invoice.sprn`
**file:script.lua
`cat ../scripts/$luascript`"

csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
. ./output_tst_libxml2.sh

