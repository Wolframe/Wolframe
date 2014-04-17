#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=$testname						# input document name
docout=$testname					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:script.lua
`cat ../scripts/$luascript`"
csetlist="UTF-8 UTF-16BE"
. ./output_tst_json.sh
