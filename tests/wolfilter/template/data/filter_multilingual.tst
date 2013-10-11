#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd='- '						# command to execute by the test
testscripts=""						# list of scripts of the test
docin=`echo $testname | sed 's/filter_//'`		# input document name
docout=$docin						# output document name
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE UCS-4LE"
. ./output_tst_textwolf.sh
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
. ./output_tst_libxml2.sh
