#!/bin/sh
TOP=../../../../
MAIN=$TOP/docs/examples/tutorial/
for example in 1 2 3 5; do

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$example.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**requires:LUA
**input
HELLO
RUN
!TEST
cat $MAIN/lua_script_$example.input.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
**file:example_$example.lua
!TEST
cat $MAIN/lua_script_$example.lua >> $output
recode lat1..ibmpc >> $output <<!TEST
**config
proc {
script {
	cmd RUN
	path example_$example.lua
	main run
}}
**output
OK enter cmd
!TEST
if [ $example = 2 ]; then
# LINE filter prints EOLN at output of non empty not EOLN-terminated last line
	cat $MAIN/lua_script_$example.output.xml | ../cleanInput BOM >> $output
else
	cat $MAIN/lua_script_$example.output.xml | ../cleanInput BOM EOLN >> $output
fi
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
**end

!TEST
done
