#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
schema=tdl_preprocess/schema_postgresql.sql
luascript=tdl_preprocess/preprocess.lua
direcmap=tdl_preprocess/preprocess.dmap
ddl=tdl_preprocess/preprocess.sfrm
tdl=tdl_preprocess/preprocess.tdl
normalize=tdl_preprocess/preprocess.wnmp

# Modules to load
modpath="../../src/modules"				# standard module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
opt="$opt --module $modpath/cmdbind/directmap/mod_command_directmap"
opt="$opt --module $modpath/normalize/number/mod_normalize_number"
opt="$opt --module $modpath/normalize/string/mod_normalize_string"
opt="$opt --module $modpath/ddlcompiler/simpleform/mod_ddlcompiler_simpleform"
modpath="../wolfilter/modules/database"			# test module directory relative from tests/temp
opt="$opt --module $modpath/postgresql/mod_db_postgresqltest"

# Programs to load
opt="$opt --cmdprogram=preprocess.dmap"			# Command map
opt="$opt --program=preprocess.sfrm"			# DDL
opt="$opt --program=preprocess.tdl"			# TDL
opt="$opt --program=preprocess.wnmp"			# normalization for DDL types
opt="$opt --program=preprocess.lua"			# Lua commands

opt="$opt --database 'identifier=testdb,host=localhost,port=5432,database=wolframe,user=wolfusr,password=wolfpwd,dumpfile=DBDUMP,inputfile=DBDATA'"
testcmd="$opt run"					# command to execute by the test
testscripts=""						# list of scripts of the test
docin=tdl_preprocess.in					# input document name
docout=tdl_preprocess.out				# output document name
dumpout="program/tdl_preprocess/postgresql.dbdump.txt"	# resource dump to add to expected test output

testdata="
**file:preprocess.sfrm
`cat program/$ddl`
**file:preprocess.wnmp
`cat program/$normalize`
**file: DBDATA
`cat program/$schema`
**file:preprocess.tdl
`cat program/$tdl`
**file:preprocess.dmap
`cat program/$direcmap`
**file:preprocess.lua
`cat program/$luascript`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
