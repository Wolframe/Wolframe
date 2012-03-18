#!/bin/sh
for example in\
	"timetrack/customer_query_1.xml=timetrack/customer.frm=customer_query"\
	"timetrack/customer_query_2.xml=timetrack/customer.frm=customer_query"
do
query=`echo $example | cut -f1 -d=`
frmfile=`echo $example | cut -f2 -d=`
frmname=`echo $example | cut -f3 -d=`
tstname=`basename $query .xml`

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$tstname.tst"
rm -f $output
echo "Expanding test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--requires:DISABLED
--input
HELLO
RUN
!TEST
./xmltestdoc.sh $cset $testdoc | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:textwolf")
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
!TEST
./xmltestdoc.sh $cset $testdoc NOEOL | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
--end

!TEST
done
