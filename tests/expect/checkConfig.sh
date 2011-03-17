#!/bin/sh

cat configTest.lst | ( while read conf ret msg; do
        echo "shell script: expect script called with: $ret, $msg, $conf"
        ./checkConfig $conf $ret "$msg"
    done )
