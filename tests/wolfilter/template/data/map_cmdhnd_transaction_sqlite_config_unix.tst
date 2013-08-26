#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
testcmd="--config wolframe.conf run"			# command to execute by the test
docin=schema_select_task_by_id				# input document name
docout=output_schema_select_task_by_id			# output document name
dumpout="program/schema_select_task_by_id.dbdump.txt"	# resource dump to add to expected test output
testdata="
**requires:DISABLED WIN32
**file: schema_select_task_by_id.sfrm
`cat ../scripts/schema_select_task_by_id.sfrm`
**file: test.dmap
`cat program/schema_select_task_by_id.dmap`
**file:wolframe.conf
`cat ../config/wolframe_unix.conf`
**file:simpleform.wnmp
`cat program/simpleform.wnmp`
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.tdl`
**outputfile:DBDUMP"
. ./output_tst_all.sh
