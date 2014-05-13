#!/bin/sh

input=doc/rand_bcdnumber_calc_input.txt
output=doc/rand_bcdnumber_calc_output.txt
rm -f $input
rm -f $output

createRandomDecimal() {
	numsize=$1
	echo `cat /dev/urandom | hexdump | cut -d' ' -f2-9 | head -$1 | sed 's/[a-f ]//g'  | awk '{cc = cc $0}END{print cc}'`
}

calculateResult()
{
	echo "$1" | bc | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//'  | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed 's/\\//g'
}

for xx in 1 2 3 4 5; do
	for op in '+' '-' '*' '/'; do
		for aa in 1 2 3 4 5 6; do
			for bb in 1 2 3; do
				arg1=`createRandomDecimal $aa`
				arg2=`createRandomDecimal $aa`
				echo "$arg1 $arg2 $op" >> $input
				calculateResult "$arg1 $op $arg2" >> $output
			done
		done
	done
done
for xx in 1 2 3 4 5; do
	for op in '>=' '<=' '>' '<' '==' '!='; do
		for aa in 1 2 3 4 5 6; do
			for bb in 1 2 3; do
				arg1=`createRandomDecimal $aa`
				arg2=`createRandomDecimal $aa`

				echo "$arg1 $arg2 $op" >> $input
				calculateResult "$arg1 $op $arg2" >> $output

				xx="$arg1"1
				yy="$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg1"3
				yy="$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg1"2
				yy="$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output

				xx="$arg2"1
				yy="$arg2"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg2"3
				yy="$arg2"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg2"2
				yy="$arg2"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
			done
		done
	done
done
for xx in 1 2 3 4 5; do
	for op in '/'; do
		for aa in 1 2; do
			for bb in 1 2 3 4 5 6 7 8 9 10 11 13 17 18 23 56 101 1023 2309 3443 69344 998931 101920912 171920912 201920912 241920912 301920912 341920912 4194093421 4394093421 5194093421 5394093421; do
				arg1=`createRandomDecimal $aa`
				echo "$arg1 $bb $op" >> $input
				calculateResult "$arg1 $op $bb" >> $output
			done
		done
	done
done

