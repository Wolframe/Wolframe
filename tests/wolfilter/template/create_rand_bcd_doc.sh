#!/bin/sh

input=doc/rand_bcdnumbder_calc_input.txt
output=doc/rand_bcdnumbder_calc_output.txt
rm -f $input
rm -f $output

createRandomDecimal() {
	numsize=$1
	echo `cat /dev/urandom | hexdump | cut -d' ' -f2-9 | head -$1 | sed 's/[a-f ]//g'  | awk '{cc = cc $0}END{print cc}'`
}

calculateResult()
{
	echo "$1" | bc | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed 's/\\//g'
}

for xx in 1 2 3 4 5 6 7 8 9 10; do
	for op in '+' '-'; do
		for aa in 1 2 3 4 5; do
			for bb in 1 2 3 4 5; do
				arg1=`createRandomDecimal $aa`
				arg2=`createRandomDecimal $aa`
				echo "$arg1 $arg2 $op" >> $input
				calculateResult "$arg1 $op $arg2" >> $output
			done
		done
	done
done

