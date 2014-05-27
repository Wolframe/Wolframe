#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
testcmd='- '						# command to execute by the test
testscripts=""						# list of scripts of the test
docin=$testname.in					# input document name
docout=$testname.out					# output document name

csetlist="UTF-8 UTF-16LE UTF-16BE UCS-4BE UCS-4LE"	# character set encodings to test
. ./output_tst_textwolf.sh

csetlist="UTF-8 UCS-2BE UCS-2LE"			# character set encodings to test
. ./output_tst_libxml2.sh
