#!/bin/sh
output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.tst"
rm -f $output
echo "Expanding test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--requires:LUA
--input
HELLO
RUN
!TEST
cat ../data/bible.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:textwolf")
	f.empty = false
	f.tokenize = true
	input:as( f)
	output:as( f)

	for c,t in input:get() do
		if c and not t then
			output:print( ' ') 
		end 
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
!TEST
cat ../data/bible.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
--end

!TEST
