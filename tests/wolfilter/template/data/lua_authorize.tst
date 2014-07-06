#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=$testname.in					# input document name
docout=$testname.out					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_authz`
	module `cmd/MODULE command_lua`
}
Processor
{
	program `cmd/PROGRAM authorize.lua`
	program `cmd/PROGRAM basic.authz`
	cmdhandler
	{
		lua
		{
			program authorize.lua
		}
	}
}
**file:basic.authz
`cat program/basic.authz`
**file:authorize.lua
`cat ../scripts/$luascript`"
. ./output_tst_all.sh
