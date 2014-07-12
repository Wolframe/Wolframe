#!/bin/bash

case "$0" in
	/*)
		base=`dirname $0`
		;;
	
	*)
		base=`pwd`/`dirname $0`
		;;
esac

if test $# != 2; then
	echo "Usage: expand <template file> <exanded file>" 1>&2
	exit 1
fi

TEMPLATE=$1
FILE=$2

. $base/../makefiles/gmake/platform.vars
. $base/settings.conf

eval "echo \"$(cat $TEMPLATE)\"" > $FILE
