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
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program normalize.wnmp
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
`cat ../scripts/$luascript`
**file:normalize.wnmp
name=ucname;
"
csetlist="UTF-8 UCS-2LE UCS-4LE"			# character set encodings to test
. ./output_tst_textwolf.sh
csetlist="UTF-8 UTF-16BE UTF-16LE"			# character set encodings to test
. ./output_tst_libxml2.sh
