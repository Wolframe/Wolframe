#!/bin/sh

# THIS FILE IS NOT PART OF WOLFRAME !
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.

echo "#!/bin/sh" > ../../testWolfilter.sh
echo "export LD_LIBRARY_PATH=../src/libwolframe" >> ../../testWolfilter.sh

for ff in `find data -type f -name "*.tst"`; do
	echo Expanding template $ff
	ii=`expr 0$ii + 1`
	./$ff $ii
done

chmod a+x ../../testWolfilter.sh
