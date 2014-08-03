**
**requires:LUA
**requires:TEXTWOLF
**input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>

.
QUIT
**file:example_3.lua

function run( )
	f = provider.filter( "textwolf")
	input:as(f)
	output:as(f)
	for c,t in input:get() do
		output:print( c, t)
	end
end
**config
provider
{
	cmdhandler
	{
		lua
		{
			program example_3.lua
			filter char
		}
	}
}
proc
{
	cmd run
}
**output
OK enter cmd
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>

.
OK
BYE
**end

