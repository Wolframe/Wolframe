#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
fltmodpath="../../src/modules/filter/"			# module directory relative from tests/temp
libxml2mod="$fltmodpath/libxml2/mod_filter_libxml2.so"
textwolfmod="$fltmodpath/textwolf/mod_filter_textwolf.so"
modopt="--module $libxml2mod --module $textwolfmod"	# list of modules to load
testcmd="$modopt --script $luascript run"		# command to execute by the test
docin=employee_assignment_print				# input document name
docout=$docin						# output document name
csetlist="UTF-8 UTF-16LE UTF-16BE"			# character set encodings to test
. ./output_tst_libxml2.sh
