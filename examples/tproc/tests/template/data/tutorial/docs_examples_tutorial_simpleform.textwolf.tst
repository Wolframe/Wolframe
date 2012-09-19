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
provider {
	cmdhandler {
		form
		{
			cmd {
				inputform input.simpleform
				inputform output.simpleform
				name run
			}
		}
	}
}
proc {
	cmd run
}
**output
OK enter cmd
!TEST
cat $MAIN/$example.input.xml | ../cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
**end

!TEST
done
