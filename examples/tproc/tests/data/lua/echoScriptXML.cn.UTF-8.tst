--
--requires:LUA
--requires:DISABLED
--input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>æ</firstname><surname>å§</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:libxml2")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
script {
	name RUN
	path echo.lua
	main run
}
--output
OK enter cmd
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<employee><firstname>æ</firstname><surname>å§</surname><phone>021436587</phone><email>yao.ming@gmail.com</email><jobtitle>sportsman</jobtitle></employee>

.
OK
OK enter cmd
BYE
--end

