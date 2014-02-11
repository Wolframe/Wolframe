#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
modpath="../../src/modules/normalize/"			# module directory for normalizers relative from tests/temp
mod="$modpath/string/mod_normalize_string"		# module to load
opt="$opt --module $mod"
luascript=`echo $testname | sed 's/lua_//'`.lua
ddltypeprg="types.wnmp"
opt="$opt --program $ddltypeprg"			# normalization program for simpleform ddl types
opt="$opt --cmdprogram $luascript"			# lua script to run
testscripts=$luascript					# list of scripts of the test
testcmd="$opt run"					# command to execute by the test
docin=$testname.in					# input document name
docout=$testname.out					# output document name
testdata="
**file:$ddltypeprg
name=ucname;
"
csetlist="UTF-8 UCS-2LE UCS-4LE"			# character set encodings to test
. ./output_tst_textwolf.sh
csetlist="UTF-8 UTF-16BE UTF-16LE"			# character set encodings to test
. ./output_tst_libxml2.sh
