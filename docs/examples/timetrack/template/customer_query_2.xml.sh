#!/bin/sh
output=`basename $0 | sed 's/.sh$//'`
./xmltestdoc.sh 'UTF-16' '<customer><name>&#x59DA; &#x660E;</name><country>china</country></customer>' > ../$output
