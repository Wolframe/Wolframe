#!/bin/sh

#
# THIS FILE IS NOT PART OF WOLFRAME !
#
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.
#
# DESCRIPTION
# - Same as output_tst_all.sh but for one ISO-8859 character set encoding of a specific code page
#
# VARIABLES
# - testname		name of the test
# - testcmd		command to execute by the test
# - docin		input document name
# - docout		output document name
# - modules		modules to load
# - codepage		ISO-8859 codepage to use
#
csetlist="ISO-8859-$codepage"
disabled=1
. ./output_tst_textwolf.sh
. ./output_tst_libxml2.sh

