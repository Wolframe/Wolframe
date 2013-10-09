#!/bin/sh

#
# THIS FILE IS NOT PART OF WOLFRAME !
#
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.
#
# DESCRIPTION
# - Script called in the context of a test script (as ". output_tst.sh") with the following variables set
#
# VARIABLES
# - testname		name of the test
# - testcmd		command to execute by the test
# - cset		character set encoding name
# - testscripts		list of scripts of the test
# - docin		input document name
# - docout		output document name
# - dumpout		(optional) file to dump to expected output too
# - inputfilter		input filter name
# - outputfilter	output filter name
# - testdata		(optional) additionaly defined test data
#
output="../data/$testname.$cset.tst"
rm -f $output
echo "Writing test file $output"
echo '**' >> $output
if [ `echo $testname | grep -c 'lua_'` != "0" ]; then
	echo '**requires:LUA' >> $output
fi
if [ `echo $testname | grep -c '_libxml2'` != "0" ]; then
	echo '**requires:LIBXML2' >> $output
fi
if [ `echo $testname | grep -c '_json'` != "0" ]; then
	echo '**requires:CJSON' >> $output
fi
if [ `echo $testname | grep -c '_sqlite'` != "0" ]; then
	echo '**requires:SQLITE3' >> $output
fi
if [ `echo $testname | grep -c '_postgresql'` != "0" ]; then
	echo '**requires:PGSQL' >> $output
fi
if [ x"$disabled" = x"1" ]; then
	echo '**requires:DISABLED' >> $output
fi
if [ x"$expecterror" != x ]; then
	echo '**exception' >> $output
	echo "$expecterror" >> $output
fi
echo '**input' >> $output
cat doc/$docin.UTF-8.$docformat | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
echo '**config' >> $output
if [ x"$inputfilter$outputfilter" = "x" ]; then
	echo "$testcmd" >> $output
elif [ x"$inputfilter" = "x" ]; then
	echo "--output-filter $outputfilter $testcmd" >> $output
else
	echo "--input-filter $inputfilter --output-filter $outputfilter $testcmd" >> $output
fi
if [ x"$testdata" != x ]; then
	echo "$testdata" >> $output
fi
for script in $testscripts
do
	echo "**file: `basename $script`" >> $output
	cat ../scripts/$script >> $output
done
echo '**output' >> $output
if [ x"$docout" != x ]; then
	if [ -f doc/$docout.UTF-8.$docformat ]; then
		cat doc/$docout.UTF-8.$docformat | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../../../wtest/cleanInput BOM EOLN >> $output
		echo "" | recode UTF-8..$cset >> $output
	elif [ -f doc/$docout.$docformat ]; then
		cat doc/$docout.$docformat >> $output
	else
		echo "OUTPUT FILE doc/$docout.UTF-8.$docformat OR doc/$docout.$docformat NOT FOUND !"
	fi
fi
if [ x"$dumpout" != "x" ]; then
	cat $dumpout >> $output
fi
echo '**end' >> $output
