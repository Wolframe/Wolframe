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
# - outputs the configuration for the tproc handler
#
# PARAM
# - $1		path of the script
#
cat <<!CFG
provider
{
	cmdhandler
	{
		lua
		{
			program $1
			filter $2
		}
	}
}
proc
{
	cmd run
}
!CFG
