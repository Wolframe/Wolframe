#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
scriptopt="--script $luascript"
testcmd="$scriptopt run"				# command to execute by the test
testscripts="$luascript"				# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=lua_ddlform_iterator				# output document name
. ./output_tst_all.sh
