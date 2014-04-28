#!/bin/sh

input=doc/rand_bcdfxpnumber_calc_input.txt
output=doc/rand_bcdfxpnumber_calc_output.txt
rm -f $input
rm -f $output

createRandomDecimal() {
	numsize=$1
	echo `cat /dev/urandom | hexdump | cut -d' ' -f2-9 | head -$1 | sed 's/[a-f ]//g'  | awk '{cc = cc $0}END{print cc}'`
}

calculateResult()
{
	if [ x"$2" = x ]; then
		echo "$1" | bc | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//'  | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed 's/\\//g'
	else
		echo "scale=$2; $1" | bc | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//'  | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed '/\\$/N;s/\n//' | sed 's/\\//g'
	fi
}

for xx in 1 2 3; do
	for op in '+' '-' '*'; do
		for aa in 1 2 3 4 5; do
			for bb in 1 2 3 4 5; do
				arg1=`createRandomDecimal $aa`
				arg2=`createRandomDecimal $bb`
				arg3=`createRandomDecimal $aa`
				arg4=`createRandomDecimal $xx`

				scale=`expr length "$arg1"`
				echo "0.$arg1 $arg2 $op" >> $input
				calculateResult "0.$arg1 $op $arg2" $scale >> $output

				scale=`expr length "$arg2"`
				echo "$arg1 0.$arg2 $op" >> $input
				calculateResult "$arg1 $op 0.$arg2" $scale >> $output

				scale=`expr length "$arg1$arg2"`
				echo "$arg3.$arg1 $arg4.$arg2 $op" >> $input
				calculateResult "$arg3.$arg1 $op $arg4.$arg2" $scale >> $output
			done
		done
	done
done
for xx in 1 2 3; do
	for op in '>=' '<=' '>' '<' '==' '!='; do
		for aa in 1 2 3 4 5; do
			for bb in 1 2 3 4 5; do
				arg1=`createRandomDecimal $aa`
				arg2=`createRandomDecimal $aa`
				arg3=`createRandomDecimal $aa`
				arg4=`createRandomDecimal $aa`
				echo "$arg1 $arg2 $op" >> $input
				calculateResult "$arg1 $op $arg2" >> $output
				echo "$arg3.$arg1 $arg4.$arg2 $op" >> $input
				calculateResult "$arg3.$arg1 $op $arg4.$arg2" >> $output

				xx="$arg3.$arg1"1
				yy="$arg3.$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg4.$arg1"3
				yy="$arg4.$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
				xx="$arg4.$arg1"2
				yy="$arg4.$arg1"2
				echo "$xx $yy $op" >> $input
				calculateResult "$xx $op $yy" >> $output
			done
		done
	done
done

