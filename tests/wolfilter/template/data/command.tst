#!/bin/sh
for example in\
	"filter=multilingual=UTF-8=- xml:textwolf" \
	"filter=multilingual=UTF-16LE=- xml:textwolf" \
	"filter=multilingual=UTF-16BE=- xml:textwolf" \
	"filter=multilingual=UCS-2LE=- xml:textwolf" \
	"filter=multilingual=UCS-2BE=- xml:textwolf" \
	"filter=multilingual=UCS-4LE=- xml:textwolf" \
        "filter=multilingual=UCS-4BE=- xml:textwolf"
do
name=`echo $example | cut -f1 -d=`
doc=`echo $example | cut -f2 -d=`
cset=`echo $example | cut -f3 -d=`
cmd=`echo $example | cut -f4 -d=`

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$name.$doc.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--input
!TEST
cat doc/$doc.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
cat >> $output <<!TEST
--config
!TEST
echo $cmd >> $output
recode lat1..ibmpc >> $output <<!TEST
--output
!TEST
cat doc/$doc.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
cat >> $output <<!TEST
--end
!TEST
done
