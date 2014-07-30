#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=authorize.in					# input document name
docout=authorize.out					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_aamap`
	module `cmd/MODULE command_lua`
}
Processor
{
	program `cmd/PROGRAM basic.aamap`
	program `cmd/PROGRAM authorize.lua`
	cmdhandler
	{
		lua
		{
			program authorize_test.lua
			filter #FILTER#
		}
	}
}
**file:basic.aamap
`cat program/basic.aamap`
**file:authorize_test.lua
`cat ../scripts/authorize_test.lua`
**file:authorize.lua
`cat ../scripts/authorize.lua`"
. ./output_tst_all.sh
