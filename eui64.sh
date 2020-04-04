#!/bin/bash
#parameter 1: eui64 common, 2: the list of file
EUI64=00:00:00:FF:FE:00

#declare -a LASTTWOBYTE
#LASTTWOBYTE=$(grep $1 | awk '{ printf $1;}')
LASTTWOBYTE=$(cat $1)
#LASTTWOBYTE[0]=B0
#LASTTWOBYTE[1]=B1
#LASTTWOBYTE[2]=B2

total=0
echo ${LASTTWOBYTE[@]}
#for i in ${LASTTWOBYTE[@]}
for i in $LASTTWOBYTE
do
	echo $i
	x=0
	while [ $x -lt 100 ] ;
	do
		x=$(printf %02d $x)
		echo $i:$x
		ret=$(grep -c -w -i $EUI64:$i:$x $2)
		if [ $ret -eq 0 ] ; then
			echo $EUI64:$i:$x	
		else
			total=`echo "$total + 1" | bc`
		fi
		x=`echo "$x + 1" | bc`
	done
done
echo total:$total
