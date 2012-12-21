#!/bin/sh

encoding=$1
doc=$2
hdr="<?xml version=\"1.0\" encoding=\"$encoding\" standalone=\"yes\"?>"

echo "$hdr" | recode XML-standalone..$encoding
echo "$doc" | recode XML-standalone..$encoding
