**
**requires:LUA
**input
HELLO
RUN
Y�  f 
 y a o   m i n g
.
QUIT
**file:example_1.lua

function run( )
        input:as( filter( "char:UTF-16BE"))
        output:as( filter( "char:UTF-16BE"))
        for c in input:get() do
                output:print( c)
        end
end
**config
script {
	name RUN
	path example_1.lua
	main run
}
**output
OK enter cmd
Y�  f 
 y a o   m i n g
.
OK
BYE
**end

