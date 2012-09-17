#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules/transaction/"		# transaction function module directory relative from tests/temp
mod="$modpath/preparedStatement/mod_transaction_pstm"	# transaction function module to load
opt="$opt --module $mod"
luascript=`echo $testname | sed 's/lua_//'`.lua
opt="$opt --script $luascript"
testcmd="$opt run"					# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=lua_ddlform_iterator				# output document name
disabled=1
. ./output_tst_all.sh
