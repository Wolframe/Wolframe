**
**requires:LUA
**requires:TEXTWOLF
**input
HELLO
RUN
姚 明
yao ming

.
QUIT
**file:example_2.lua

function run( )
        local f = provider.filter( "line", "UTF-8")
        input:as( f)
        output:as( f)
        for c in input:get() do
                output:print( c)
        end
end
**config
provider
{
	cmdhandler
	{
		lua
		{
			program example_2.lua
		}
	}
}
proc
{
	cmd run
}
**output
OK enter cmd
姚 明
yao ming

.
OK
BYE
**end

