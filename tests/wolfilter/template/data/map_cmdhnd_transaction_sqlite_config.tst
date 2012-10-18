#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
ddltypeprg="simpleform.normalize"
testcmd="--config wolframe.conf run"					# command to execute by the test
docin=schema_select_task_by_id				# input document name
docout=output_schema_select_task_by_id			# output document name
dumpout="program/schema_select_task_by_id.dbdump.txt"	# resource dump to add to expected test output
testdata="
**file:wolframe.conf
`cat ../config/wolframe.conf`
**file:simpleform.normalize
`cat program/$ddltypeprg`
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.tdl`
**outputfile:DBDUMP"
. ./output_tst_all.sh
