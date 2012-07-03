#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
formname="employee_assignment_print.simpleform"
fid=`basename $formname .simpleform`
funcopt="--function transaction:echo_peer:$fid:$fid"
formopt="--form $formname"
scriptopt="--script $luascript"
testcmd="$formopt $funcopt $scriptopt run"		# command to execute by the test
testscripts="$luascript $formname"			# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=lua_ddlform_input_generator			# output document name
. ./output_tst_all.sh
