#!/bin/sh

module="MAC RPL DHCP UIP DM MPL 6LOWPAN DS6 COAP APPS SEC DRV ATCMD SYS OTA ETH MCU UART RF PLC" 
count=0
para=$(awk -F'[][]' '{
		if ( NR == 1) {
			printf "%s %s %s", $2, $4, $6;
		}
	}' aaa)
for i in ${para[@]}
do
	echo $i
	if [ $count -eq 0 ] ; then
		if [ "$i" == INT ] || [ "$i" == REL ] ; then
			echo OK
		else
			echo error
			exit 1
		fi
	elif [ $count -eq 1 ] ; then
		if [ "$i" == FIXED ] || [ "$i" == FEATURE ] ; then
			echo OK
		else
			echo error
			exit 1
		fi
	elif [ $count -eq 2 ] ; then
		flag=0
		for j in ${module[@]}
		do
			if [ "$i" == "$j" ] ; then
				echo OK
				flag=1
				break
			fi
		done
		if [ $flag -eq 0 ] ; then
			echo error
			exit 1
		fi
	fi
	let count+=1
	echo $count
done
exit 0
#for mod in ${module[@]}
#do
#	echo $para
#	echo $mod
#done

