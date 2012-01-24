#!/bin/bash

for i in {01..48}
do
	ping -c 1 "ensipsys$i" > /dev/null;
	if [ $? -eq 0 ]; then
		echo "ensipsys$i" >> machines
	fi
done
