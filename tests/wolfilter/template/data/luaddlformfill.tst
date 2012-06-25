#!/bin/sh
for example in\
	"employee_assignment_print=employee_assignment_print_res=UTF-8=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UTF-16LE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UTF-16BE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-2LE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-2BE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-4LE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform" \
	"employee_assignment_print=employee_assignment_print_res=UCS-4BE=run xml:textwolf=ddlform_luatable_luaparse.lua=employee_assignment_print.simpleform"
do
docin=`echo $example | cut -f1 -d=`".luaddlformfill"
docout=`echo $example | cut -f2 -d=`".luaddlformfill"
cset=`echo $example | cut -f3 -d=`
cmdnam=`echo $example | cut -f4 -d=`
script=`echo $example | cut -f5 -d=`
form=`echo $example | cut -f6 -d=`
cmd="--form $form --script $script $cmdnam"

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$docin.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**input
!TEST
cat doc/$docin.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo "" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM >> $output
echo "**file: $script" >> $output
cat ../scripts/$script >> $output
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
