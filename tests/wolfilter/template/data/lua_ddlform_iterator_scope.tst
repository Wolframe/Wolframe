#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testcmd="-c wolframe.conf run"				# command to execute by the test
docin=employee_assignment_print				# input document name
docout=$testname					# output document name
testdata="
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
			filter #FILTER#
		}
	}
}
**file:normalize.wnmp
`cat program/simpleform_range_trim.wnmp`
**file:form.sfrm
`cat ../scripts/employee_assignment_print.sfrm`
**file:script.lua
`cat ../scripts/$luascript`"
. ./output_tst_all.sh
