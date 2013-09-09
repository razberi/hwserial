#!/bin/bash

count=0;
nullchar=`echo -ne "\0"`
closebrace="}"
echo -n "" > dump #clear file
while read -n 1 c; do
	echo -ne "$c" | tee -a dump >&2
	#if [ "$c" = "$nullchar" ]; then
	if [ "$c" = "$closebrace" ]; then
		#echo "null char detected after receiving $count chars, sending json response..."
		sleep 1
		cat jsonData.json
		sleep 10
		count=0
		break
	else
		#echo "$c not equal to $nullchar"
		((count++))
	fi
done

#echo "$count chars not equal to \0"
