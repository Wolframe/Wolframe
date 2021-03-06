**
**requires:LUA
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < i m a g e   i d = " 1 " / > < i m a g e   i d = " 1 "   n a m e = " t r o l l " / > < i m a g e   i d = " 2 " > < / i m a g e > < i m a g e   i d = " 3 " > X < / i m a g e > < / d o c >**config
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
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < i m a g e   i d = " 1 " / > < i m a g e   i d = " 1 "   n a m e = " t r o l l " / > < i m a g e   i d = " 2 " / > < i m a g e   i d = " 3 " > X < / i m a g e > < / d o c > 
**end
