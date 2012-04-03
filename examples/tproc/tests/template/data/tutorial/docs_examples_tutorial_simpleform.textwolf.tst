#!/bin/sh
TOP=../../../../
MAIN=$TOP/docs/examples/tutorial/
for example in\
	ddl_simpleform_1
do
output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$example.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**requires:DISABLED
**input
HELLO
RUN
!TEST
cat $MAIN/$example.input.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
**file:input.frm
!TEST
cat $MAIN/$example.frm >> $output
recode lat1..ibmpc >> $output <<!TEST
**file:output.frm
!TEST
cat $MAIN/$example.frm >> $output
recode lat1..ibmpc >> $output <<!TEST
**config
proc {
directmap {
	cmd RUN
	ddl simpleform
	filter XML:textwolf
	inputform input.frm
	outputform output.frm
	function echo
}}
**output
OK enter cmd
!TEST
cat $MAIN/$example.input.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
**end

!TEST
done
