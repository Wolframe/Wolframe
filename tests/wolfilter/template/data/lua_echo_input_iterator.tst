#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
docin=employee_assignment_print				# input document name
docout=$docin						# output document name

testcmd="$opt --cmdprogram $luascript run"		# command to execute by the test
csetlist="UCS-2LE UCS-2BE UCS-4BE UCS-4LE"		# character set encodings to test
. ./output_tst_textwolf.sh

testcmd="$opt --cmdprogram $luascript run"		# command to execute by the test
csetlist="UTF-8 UTF-16LE UTF-16BE"			# character set encodings to test
. ./output_tst_libxml2.sh
