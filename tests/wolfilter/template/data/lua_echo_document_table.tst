#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=employee_assignment_print_2			# input document name
docout=$testname					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/filter/blob/mod_filter_blob
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
			filter #FILTER#
		}
	}
}
**file:script.lua
`cat ../scripts/$luascript`"
csetlist="UTF-8"
. ./output_tst_all.sh
