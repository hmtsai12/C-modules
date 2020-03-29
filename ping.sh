#!/bin/sh
NETWORK=8.8.8
#IP=8.8.8.8
#x=0
#while [ $x -lt 10 ]
#do
#	echo $x
#	x='echo "$x + 1" | bc'
#done
for ip in $(seq 1 $1)
do
	ping $NETWORK.$ip -c 1 -W $2 > ping.log
	#echo $?
	if [ $? -eq 0 ] ; then
		cat ping.log | head -2 | grep time | awk '{ print $4 $7 $8 ; }'
	else
		echo "$NETWORK.$ip:timeout"
	fi
done
