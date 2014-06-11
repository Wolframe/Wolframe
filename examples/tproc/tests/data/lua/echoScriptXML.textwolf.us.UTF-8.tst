--
--requires:TEXTWOLF
--requires:LUA
--input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>
.
QUIT
--file:echo.lua
function run( )
	f = provider.filter( "textwolf")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
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
--output
OK enter cmd
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>

.
OK
BYE
--end

