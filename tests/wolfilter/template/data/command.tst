#!/bin/sh
for example in\
	"filter=multilingual=multilingual=UTF-8=- xml:textwolf" \
	"filter=multilingual=multilingual=UTF-16LE=- xml:textwolf" \
	"filter=multilingual=multilingual=UTF-16BE=- xml:textwolf" \
	"filter=multilingual=multilingual=UCS-2LE=- xml:textwolf" \
	"filter=multilingual=multilingual=UCS-2BE=- xml:textwolf" \
	"filter=multilingual=multilingual=UCS-4LE=- xml:textwolf" \
	"filter=multilingual=multilingual=UCS-4BE=- xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UTF-8=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UTF-16LE=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UTF-16BE=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UCS-2LE=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UCS-2BE=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UCS-4LE=employee_assignment_convert xml:textwolf" \
	"filter=employee_assignment_print=employee_assignment_print_res=UCS-4BE=employee_assignment_convert xml:textwolf"
do
name=`echo $example | cut -f1 -d=`
docin=`echo $example | cut -f2 -d=`
docout=`echo $example | cut -f3 -d=`
cset=`echo $example | cut -f4 -d=`
cmd=`echo $example | cut -f5 -d=`

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$name.$docin.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--input
!TEST
cat doc/$docin.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
cat >> $output <<!TEST
--config
!TEST
echo $cmd >> $output
recode lat1..ibmpc >> $output <<!TEST
--output
!TEST
cat doc/$docout.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset >> $output
cat >> $output <<!TEST
--end
!TEST
done
