#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
inputfilter="employee_assignment_print_map"		# XSLT script as input filter
docin=`echo $testname | sed 's/filter_//' | sed 's/_xslt//'`		# input document name
docout="employee_assignment_print_xslt.out"		# output document name
testcmd="-c wolframe.conf -"				# command to execute by the test
testdata="
**requires:LIBXSLT
**file:wolframe.conf
Processor
{
	program employee_assignment_print_map.xslt
}
**file:employee_assignment_print_map.xslt
`cat program/employee_assignment_print_map.xslt`"
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
. ./output_tst_libxml2.sh

