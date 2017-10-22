#!/bin/bash

COUNT=0
while [ 1 ]; do
	echo "################# RUN # ${COUNT} ######################"
	./poller.py
	if [ $? -ne 0 ]; then
		exit
	fi
	echo
	echo
	echo
	COUNT=$((COUNT + 1))
done
