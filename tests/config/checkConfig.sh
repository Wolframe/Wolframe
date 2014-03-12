#!/bin/sh

EXPECT=$1

cat configTests.lst | ( while read conf ret msg; do
#        echo "shell script: expect script called with: $ret, $msg, $conf"
        $EXPECT ./checkConfig.ex $conf $ret "$msg"
        if [ "$?" != "0" ]; then
                exit 1
        fi
    done )
