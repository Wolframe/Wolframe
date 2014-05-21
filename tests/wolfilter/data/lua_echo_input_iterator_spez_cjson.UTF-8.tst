**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
    "image": [
      { "-id": "1" },
      {
        "-id": "1",
        "-name": "troll"
      },
      { "-id": "2" },
      {
        "-id": "3",
        "#text": "X"
      }
    ]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

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
{
  "image":  [{
      "id":  "1"
    }, {
      "id":  "1",
      "name":  "troll"
    }, {
      "id":  "2"
    }, {
      "id":  "3",
      "#text":  "X"
    }]
}
**end
