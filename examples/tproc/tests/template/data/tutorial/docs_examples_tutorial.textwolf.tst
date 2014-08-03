#!/bin/sh
TOP=../../../../
MAIN=$TOP/docs/examples/AppDevelop/
for example in 1 2 3 5; do

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$example.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**requires:LUA
**requires:TEXTWOLF
**input
HELLO
RUN
!TEST
cat $MAIN/lua_script_$example.input.xml | ../cleanInput BOM >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
**file:example_$example.lua
!TEST
cat $MAIN/lua_script_$example.lua >> $output
recode lat1..ibmpc >> $output <<!TEST
**config
!TEST
./output_provider_cfg.sh example_$example.lua char >> $output
recode lat1..ibmpc >> $output <<!TEST
**output
OK enter cmd
!TEST
cat $MAIN/lua_script_$example.output.xml | ../cleanInput BOM >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
**end

!TEST
done
