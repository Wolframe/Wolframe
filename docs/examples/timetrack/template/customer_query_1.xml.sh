#!/bin/sh
output=`basename $0 | sed 's/.sh$//'`
./xmltestdoc.sh 'UTF-8' '<customer><name>Hans Muster</name></customer>' > ../$output
