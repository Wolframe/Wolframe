#!/bin/sh
for example in\
	"UCS-4BE=us=employee Sara Sample 0123456789 sara.sample@gmail.com engineer"\
	"UCS-4BE=cn=employee  &#x59DA; &#x660E; 021436587 yao.ming@gmail.com sportsman"
do
# yao ming UTF-8: &#xE6&#x98&#x8E &#xE5&#xA7&#x98
cset=`echo $example | cut -f1 -d=`
tag=`echo $example | cut -f2 -d=`
testdoc=`echo $example | cut -f3 -d=`

output="../`echo $0 | sed 's/template//' | sed 's/\.4Byte//' | sed 's/.tst$//'`.$tag.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--requires:LUA
--requires:DISABLED NETBSD
--requires:LIBXML2
--input
HELLO
RUN
!TEST
./xmltestdoc.sh $cset $testdoc | ../cleanInput BOM >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
--file:echo.lua
function run( )
	f = filter( "libxml2")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
!TEST
./output_provider_cfg.sh echo.lua >> $output
recode lat1..ibmpc >> $output <<!TEST
--output
OK enter cmd
!TEST
./xmltestdoc.sh $cset $testdoc | ../cleanInput BOM >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
--end

!TEST
done
