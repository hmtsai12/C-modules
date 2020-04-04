#!/bin/sh
#parameter 1: last ip, 2: the time(s) of timeout 3: loop counter
#NETWORK=8.8.8
NETWORK=2001:db8::
PRASE="cat ping.log | head -2 | grep time | awk '{ print $4 $7 $8 ; }'"
#IP=8.8.8.8
x=0
infinite=0
loop=$3
number=$(($1 + 2))
#while [ $x -lt 10 ]
#do
#	echo $x
#	x='echo "$x + 1" | bc'
#done
EndResult() {
	TestResult
	exit
}

TestResult() {
	for ip in $(seq 3 $number)
	do
		hexip=$(echo "obase=16;$ip" | bc)
		sum=$(grep "$NETWORK$hexip: time" result.log | awk -F '[=]' '{ printf "%d ", $2}')
		sumcount=$(grep -c "$NETWORK$hexip: time" result.log)
		sumfail=$(grep -c -w $NETWORK$hexip:timeout result.log)
		#echo $sum
		cal_sum=0
		for i in ${sum[@]}
		do
			let cal_sum+=$i
		done
		#echo $cal_sum
		#echo $sumcount
		#echo $sumfail
		#echo $NETWORK$hexip:
		printf "%s%s:" $NETWORK $hexip
		if [ $sumcount -ne 0 ] ; then
			echo $cal_sum $sumcount $sumfail | awk '{ printf "avg:%.4fms PER:%.4f\n", $1/$2 , $3/$2*100}'
		else
			echo -e "\E[0;31;40m all time out \e[0m"
		fi	
	done
	EndTotal=$(grep -c $NETWORK result.log)
	EndFail=$(grep -c :timeout result.log)
	echo exit
	echo Fail:$EndFail
	echo Total:$EndTotal
	echo $EndFail $EndTotal | awk '{ printf "PER:%.4f\n", $1/$2*100 }'
}
#trap "echo exit; exit" 2
trap EndResult 2
#trap "echo 1" 1
if [ -f result.log ] ; then
	mv result.log result.log.bak
fi
if [ $loop -eq 0 ] ; then
	x=-1
fi
#rm -f result.log
while [ "$x" -lt $loop ] ;
do
	if [ $loop -eq 0 ] ; then
		echo $infinite
		infinite=`echo "$infinite + 1" | bc`
	else
		echo $x
		x=`echo "$x + 1" | bc`
	fi
	# ping function
	for ip in $(seq 3 $number)
	do
		hexip=$(echo "obase=16;$ip" | bc)
		timestart=$(($(date +%s%N)/1000000))
		#ping6 $NETWORK$ip -c 1 -W $2 > ping.log
		ping6 $NETWORK$hexip -c 1 -W $2 > ping.log
		retval=$?
		timeend=$(($(date +%s%N)/1000000))
		#echo $?
		if [ $? -eq $retval ] ; then
			#PASSRESULT=$(cat ping.log | head -2 | grep time | awk '{ printf "%s %s%s", $4 , $7 , $8 ; }')
			PASSRESULT=$(cat ping.log | head -2 | grep time | awk '{ printf "%s", $4 ; }')
			#DATE=$(date '+%Y%m%d%H%M%S') >> result.log
			DATE=$(date '+%Y%m%d%H%M%S.%N')
			#cat ping.log | head -2 | grep time | awk '{ print $4 $7 $8 ; }'
			#cat ping.log | head -2 | grep time | awk '{ print $4 $7 $8 ; }' >> result.log
			#eval $PRASE
			#PASSRESULT=$(eval $PRASE)
			during=$((timeend - timestart))
			#echo $during
			echo "$DATE $PASSRESULT time=$during" >> result.log
			TOTAL=$(grep -c -w $NETWORK$hexip result.log)
			FAIL=$(grep -c -w $NETWORK$hexip:timeout result.log)
			PASS=$(grep -c -w $NETWORK$hexip: result.log)
			echo "$DATE $NETWORK$hexip time=$during ms($PASS:$FAIL:$TOTAL)"
			#cat ping.log | tail -1 | awk -F '[/ ]' '{ print $2 $7 $8 ; }'
		else
			#printf "\E[0;31;47m"
			DATE=$(date '+%Y%m%d%H%M%S.%N')
			echo "$DATE $NETWORK$hexip:timeout" >> result.log
			TOTAL=$(grep -c -w $NETWORK$hexip result.log)
			PASS=$(grep -c -w $NETWORK$hexip:time= result.log)
			FAIL=$(grep -c -w $NETWORK$hexip:timeout result.log)
			echo -e "\E[0;31;40m$NETWORK$hexip:timeout \e[0m"
			echo "$DATE $NETWORK$hexip ($PASS:$FAIL:$TOTAL)"
		fi
	done
done
TestResult
#EndTotal=$(grep -c $NETWORK result.log)
#EndFail=$(grep -c :timeout result.log)
#echo exit
#echo Fail:$EndFail
#echo Total:$EndTotal
#echo $EndFail $EndTotal | awk '{ printf "PER:%.4f\n", $1/$2 }'

