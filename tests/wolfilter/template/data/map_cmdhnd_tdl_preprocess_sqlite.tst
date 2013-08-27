#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
schema=tdl_preprocess/schema_sqlite.sql
luascript=tdl_preprocess/preprocess.lua
direcmap=tdl_preprocess/preprocess.dmap
ddl=tdl_preprocess/preprocess.sfrm
tdl=tdl_preprocess/preprocess.tdl
normalize=tdl_preprocess/preprocess.wnmp

# Modules to load
modpath="../../src/modules"				# standard module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
opt="$opt --module $modpath/cmdbind/directmap/mod_command_directmap"
opt="$opt --module $modpath/number/normalize/mod_normalize_number"
opt="$opt --module $modpath/string/normalize/mod_normalize_string"
modpath="../wolfilter/modules/database"			# test module directory relative from tests/temp
opt="$opt --module $modpath/sqlite3/mod_db_sqlite3test"

# Programs to load
opt="$opt --cmdprogram=$direcmap"			# Command map
opt="$opt --program=$ddl"				# DDL
opt="$opt --program=$tdl"				# TDL
opt="$opt --program=$normalize"				# normalization for DDL types
opt="$opt --program=$luascript"				# Lua commands

opt="$opt --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA'"
testcmd="$opt run"					# command to execute by the test
testscripts=""						# list of scripts of the test
docin=tdl_preprocess.in					# input document name
docout=tdl_preprocess.out				# output document name
dumpout="program/tdl_preprocess/sqlite.dbdump.txt"	# resource dump to add to expected test output

testdata="
**file:$normalize
`cat program/$normalize`
**file: DBDATA
`cat program/$schema`
**file:preprocess.tdl
`cat program/$tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
