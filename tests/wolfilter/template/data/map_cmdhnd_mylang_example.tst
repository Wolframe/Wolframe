#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
mylangscript=mylang/example.mlg
direcmap=mylang/example.dmap
ddl=mylang/example.sfrm
normalize=mylang/example.wnmp

# Modules to load
modpath="../../src/modules"				# standard module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/directmap/mod_command_directmap"
opt="$opt --module $modpath/normalize/number/mod_normalize_number"
opt="$opt --module $modpath/normalize/string/mod_normalize_string"
opt="$opt --module $modpath/ddlcompiler/simpleform/mod_ddlcompiler_simpleform"

# Test modules to load
modpath="../wolfilter/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/mylang/mod_command_mylang"

# Programs to load
opt="$opt --cmdprogram=example.dmap"			# Command map
opt="$opt --program=example.sfrm"			# DDL
opt="$opt --program=example.wnmp"			# normalization for DDL types
opt="$opt --program=example.mlg"			# Lua commands

testcmd="$opt run"					# command to execute by the test
testscripts=""						# list of scripts of the test
docin=cdcatalog.in					# input document name
docout=cdcatalog.out					# output document name

testdata="
**file:example.sfrm
`cat program/$ddl`
**file:example.wnmp
`cat program/$normalize`
**file:example.dmap
`cat program/$direcmap`
**file:example.mlg
`cat program/$mylangscript`"
csetlist="UTF-8"
. ./output_tst_all.sh
