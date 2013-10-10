**
**requires:LUA
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><image id="1"/><image id="1" name="troll"/><image id="2"></image><image id="3">X</image></doc>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram echo_input_iterator_spez.lua run
**requires:TEXTWOLF
**file: echo_input_iterator_spez.lua

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
			process_image( scope( itr))
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
