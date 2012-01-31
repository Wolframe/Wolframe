--
--requires:LUA
--requires:DISABLED
--input
CMD1A 1
CMD2A 2
CMD3A 2
RUN

.
QUIT
--file:echo.lua
function run( )
	for ch,t in input:get() do
		output:print( ch,t)
	end
end
--config
script {
	name RUN
	path echo.lua
	main run
}
--output
OK CMD1A '1'
OK CMD2A '2'
OK CMD3A '3'

.
BYE
--end
