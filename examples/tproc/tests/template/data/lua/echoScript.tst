#!/bin/sh

output="../`echo $0 | sed 's/template//'`"
rm -f $output
echo "Writing test file $output"
recode lat1..ibmpc >> $output <<!TEST
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
		output:print( ch,t)
		output:print( 'X')
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
!TEST
./output_provider_cfg.sh echo.lua >> $output
recode lat1..ibmpc >> $output <<!TEST
--end
!TEST
