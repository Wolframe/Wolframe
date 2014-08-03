**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><image id="1"/><image id="1" name="troll"/><image id="2"></image><image id="3">X</image></doc>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run
**requires:TEXTWOLF
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
			filter textwolf
		}
	}
}
**file:script.lua

local function process_image( itr)
	for v,t in itr do
		output:print( v, t)
	end
end

function run()
	local itr = input:get()
	for v,t in itr do
		if t == "image" then
			output:opentag( t)
			process_image( iterator.scope( itr))
			output:closetag()
		else
			output:print( v, t)
		end
	end
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><image id="1"/><image id="1" name="troll"/><image id="2"/><image id="3">X</image></doc>
**end
