#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
docin=luacall_context_in
docout=luacall_context_out
testcmd="-c wolframe.conf echoUserData"			# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE mod_command_directmap`
	module `cmd/MODULE mod_command_lua`
	module `cmd/MODULE mod_ddlcompiler_simpleform`
}
Processor
{
	program echo_input_table.lua
	program userdata.sfrm
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter #FILTER#
		}
	}
}
**file: test.dmap
COMMAND (echo UserData) CALL run CONTEXT {uname=UserName, host=RemoteHost} RETURN UserDataWithLogin;
**file:userdata.sfrm
`cat ../scripts/userdata.sfrm`
**file:echo_input_table.lua
function run( input)
	return input:get()
end
"
. ./output_tst_all.sh

