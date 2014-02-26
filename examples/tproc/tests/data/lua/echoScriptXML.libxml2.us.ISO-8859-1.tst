--
--requires:LUA
--requires:LIBXML2
--input
HELLO
RUN
<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>

.
QUIT
--file:echo.lua
function run( )
	f = provider.filter( "libxml2")
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
		}
	}
}
proc
{
	cmd run
}
--output
OK enter cmd
<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>

.
OK
BYE
--end

