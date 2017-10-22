#!/bin/bash

COUNT=0
while [ 1 ]; do
	echo "################# RUN # ${COUNT} ######################"
	./poller_v1.py 127.0.0.1 4001
	if [ $? -ne 0 ]; then
		exit
	fi
	echo
	echo
	echo
	COUNT=$((COUNT + 1))
	sleep 2
done
