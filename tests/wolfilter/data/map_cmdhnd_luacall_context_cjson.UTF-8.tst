**
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"-doctype":	"UserData",
	"data":	"bla",
	"command":	"exec"
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf echoUserData

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
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
			filter cjson
		}
	}
}
**file: test.dmap
COMMAND (echo UserData) CALL run CONTEXT {uname=UserName, host=RemoteHost} RETURN UserData;
**file:userdata.sfrm
FORM UserData
	-root doc
{
	uname string
	host string
	data string
	command string
}
**file:echo_input_table.lua
function run( input)
	return input:get()
end

**output
{
	"-doctype":	"UserData",
	"uname":	"wolfilter",
	"host":	"123.123.123.123",
	"data":	"bla",
	"command":	"exec"
}
**end
