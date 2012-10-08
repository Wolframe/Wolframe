#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../wolfilter/modules"				# module directory relative from tests/temp
mod="$modpath/transaction/echo/mod_transaction_echo"	# module to load
opt="$opt --module $mod"
opt="$opt --transaction 'echo_transaction test'"	# declare the transaction function used
luascript=`echo $testname | sed 's/lua_//'`.lua		# script to execute
opt="$opt --script $luascript"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=lua_ddlform_input_generator			# output document name
disabled=1
. ./output_tst_all.sh
