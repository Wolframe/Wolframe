testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
testcmd="--script $luascript run"			# command to execute by the test
docin=doc/rand_bcdnumbder_calc_input.txt		# input document name
docout=doc/rand_bcdnumbder_calc_output.txt		# output document name
output="../data/$testname.tst"

echo '**' > $output
echo '**input' >> $output
cat $docin >> $output
echo '**output' >> $output
cat $docout >> $output
echo '**file:'"$luascript" >> $output
cat ../scripts/$luascript >> $output
echo '**config' >> $output
echo "$testcmd" >> $output
echo '**end' >> $output

