testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
testcmd="-c wolframe.conf run"				# command to execute by the test
docin=doc/rand_bcdnumber_calc_input.txt			# input document name
docout=doc/rand_bcdnumber_calc_output.txt		# output document name
output="../data/$testname.tst"

echo '**' > $output
echo '**requires:LUA' >> $output
echo '**requires:TEXTWOLF' >> $output
echo '**file:wolframe.conf' >> $output
cat >> $output <<EOF
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/filter/line/mod_filter_line
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	cmdhandler
	{
		lua
		{
			program bcdnumber_calc.lua
		}
	}
}
EOF
echo '**input' >> $output
cat $docin >> $output
echo '**output' >> $output
cat $docout >> $output
echo '**file:'"$luascript" >> $output
cat ../scripts/$luascript >> $output
echo '**config' >> $output
echo "$testcmd" >> $output
echo '**end' >> $output

