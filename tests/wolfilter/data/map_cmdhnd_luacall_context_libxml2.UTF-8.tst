**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE doc SYSTEM "UserData.simpleform"><doc><data>bla</data><command>exec</command></doc>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf echoUserData

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
			filter libxml2
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
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE doc SYSTEM "UserData.simpleform"><doc><uname>wolfilter</uname><host>123.123.123.123</host><data>bla</data><command>exec</command></doc>
**end
