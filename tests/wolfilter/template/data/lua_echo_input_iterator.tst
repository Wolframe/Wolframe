#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=employee_assignment_print				# input document name
docout=$docin						# output document name
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
			filter #FILTER#
		}
	}
}
**file:script.lua
`cat ../scripts/$luascript`"

csetlist="UTF-8 UCS-2LE UCS-2BE UCS-4BE UCS-4LE"	# character set encodings to test
. ./output_tst_textwolf.sh

csetlist="UTF-8 UTF-16LE UTF-16BE"			# character set encodings to test
. ./output_tst_libxml2.sh
