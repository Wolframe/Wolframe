--
--requires:TEXTWOLF
--requires:LUA
--input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>姚</firstname><surname>明</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>
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
<employee><firstname>姚</firstname><surname>明</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>

.
OK
BYE
--end

