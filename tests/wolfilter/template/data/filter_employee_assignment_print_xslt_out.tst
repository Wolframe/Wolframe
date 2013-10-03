#!/bin/sh
testname=`basename $0 ".tst"`						# name of the test
opt="--program employee_assignment_print_map.xslt"
outputfilter="employee_assignment_print_map"				# XSLT script as output filter
testcmd="$opt - "							# command to execute by the test
testscripts=""								# list of scripts of the test
docin=`echo $testname | sed 's/filter_//' | sed 's/_xslt_out//'`	# input document name
docout="employee_assignment_print_xslt.out"				# output document name
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
testdata="
**requires:LIBXSLT
**file:employee_assignment_print_map.xslt
`cat program/employee_assignment_print_map.xslt`"
. ./output_tst_libxml2.sh
