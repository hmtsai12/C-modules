#!/bin/sh

module="RPL MAC APPS DRV" 
for mod in ${module[@]}
do
	echo $mod
	awk -F'[][]' '{
		print NR;
		if ( NR == 1) {
			print $6;
			if ( $6 == $mod ) {
				print $6;
			} else {
				print "error";
				print $mod;
			}
		}
	}' aaa
done

