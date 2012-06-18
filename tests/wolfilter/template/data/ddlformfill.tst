#!/bin/sh
for example in\
	"employee_assignment_print=employee_assignment_print_res=UTF-8=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UTF-16LE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UTF-16BE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-2LE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-2BE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-4LE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-4BE=employee_assignment_print xml:textwolf=employee_assignment_print.simpleform"
do
docin=`echo $example | cut -f1 -d=`".ddlformfill"
docout=`echo $example | cut -f2 -d=`".ddlformfill"
cset=`echo $example | cut -f3 -d=`
cmdnam=`echo $example | cut -f4 -d=`
form=`echo $example | cut -f5 -d=`
cmd="--form $form $cmdnam"

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$docin.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**input
!TEST
cat doc/$docin.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM >> $output
echo "**file: $form" >> $output
cat ../scripts/$form >> $output
cat >> $output <<!TEST
**config
!TEST
echo $cmd >> $output
recode lat1..ibmpc >> $output <<!TEST
**output
!TEST
cat doc/$docout.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM >> $output
cat >> $output <<!TEST
**end
!TEST
done
