testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
mod="$modpath/filter/line/mod_filter_line"		# filter module to load
opt="--module $mod"
mod="$modpath/lua/bcdnumber/mod_lua_bcdnumber"
opt="$opt --module $mod"
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
opt="$opt --cmdprogram $luascript"
testcmd="$opt run"					# command to execute by the test
docin=doc/rand_bcdnumbder_calc_input.txt		# input document name
docout=doc/rand_bcdnumbder_calc_output.txt		# output document name
output="../data/$testname.tst"

echo '**' > $output
echo '**requires:LUA' >> $output
echo '**requires:TEXTWOLF' >> $output
echo '**input' >> $output
cat $docin >> $output
echo '**output' >> $output
cat $docout >> $output
echo '**file:'"$luascript" >> $output
cat ../scripts/$luascript >> $output
echo '**config' >> $output
echo "$testcmd" >> $output
echo '**end' >> $output

