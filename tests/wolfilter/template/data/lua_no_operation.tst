#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=no_operation.in					# input document name
docout=no_operation.out					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_lua`
}
Processor
{
	cmdhandler
	{
		lua
		{
			program no_operation.lua
			filter #FILTER#
		}
	}
}
**file:no_operation.lua
`cat ../scripts/$luascript`"
. ./output_tst_all.sh
