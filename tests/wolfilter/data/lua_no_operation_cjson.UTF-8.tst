**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
    "-doctype": "Person",
    "name": "Hugi Turbo",
    "address": "Gurkenstrasse 7a 3145 Gumligen"
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program no_operation.lua
			filter cjson
		}
	}
}
**file:no_operation.lua
function run( input )
end
**output
{
	"-doctype":	"Person"
}
**end
