--
--requires:LUA
--input
CMD1A 1
CMD2A 2
CMD3A 3
RUN

.
CMD3A 4
QUIT
--file:echo.lua
function run( )
	input:as( provider.filter( "char"))
	for ch,t in input:get() do
		logger.printc( "OUTPUT [", ch, "]")
		output:print( ch,t)
		output:print( '_')
	end
end
--output
OK CMD1A '1'
OK CMD2A '2'
OK CMD3A '3'
OK enter cmd

.
OK
OK CMD3A '4'
OK enter cmd
BYE
--requires:TEXTWOLF
--config
provider
{
	cmdhandler
	{
		lua
		{
			program echo.lua
			filter char
		}
	}
}
proc
{
	cmd run
}
--end
