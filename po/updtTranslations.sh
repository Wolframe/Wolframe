#!/bin/bash

# ${DOMAIN}.pot is used as the extracted message file
DOMAIN="Wolframe"
LOCALES="ro de"
EXTENSIONS="hpp cpp"

for ext in ${EXTENSIONS}
do
	ls ../src/ | grep "\.${ext}$" |
	( while read file
	do
		echo "Processing: ${file}"
		if [ -f ${DOMAIN}.pot ]; then
			xgettext -j -d ${DOMAIN} -F -D .. -o ${DOMAIN}.pot src/${file}
		else
			xgettext -d ${DOMAIN} -F -D .. -o ${DOMAIN}.pot src/${file}
		fi
	done
	)
done

if [ -f ${DOMAIN}.pot ]; then
	for loc in ${LOCALES}; do
		echo "Merging ${DOMAIN}.pot to ${loc} message file"
		msgmerge -v -F -U ${loc}.po ${DOMAIN}.pot
	done
	rm -v ${DOMAIN}.pot
fi
