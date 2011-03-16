#!/bin/sh

cat configTest.lst | ( while read conf ret msg; do
        echo  $ret $msg $conf
        ./checkConfig $conf $ret $msg
    done )
