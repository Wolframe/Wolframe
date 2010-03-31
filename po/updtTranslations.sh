#!/bin/bash

DOMAIN="SMERP"
LOCALES="ro de"
EXTENSIONS="hpp cpp"

for ext in ${EXTENSIONS}
do
    ls ../src/ | grep "\.${ext}$" |
    ( while read file
	do
    	    xgettext -d ${DOMAIN} -s -D .. -o ${file}.pot src/${file}
            echo "Processing: ${file}"
    	    if [ -f ${file}.pot ]; then
        	    for loc in ${LOCALES}; do
        		echo "Merging ${file}.pot to ${loc} message file"
			msgmerge -s -U ${loc}.po ${file}.pot
    		    done
		    rm -v ${file}.pot
    	    fi
	done
    )
done
