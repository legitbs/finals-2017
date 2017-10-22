#!/usr/bin/python 

import sys, os
import socket
import select

def Data_8_to_9(InData):
	#don't feel like doing all the math, make a bit list then reparse it
	Bits = ""
	for i in InData:
		Bits += (("0"*9)+bin(ord(i))[2:])[-9:]

	#now add in enough bits to make sure we can send full bytes
	if len(Bits) % 8:
		Bits += "0"*(8-(len(Bits) % 8))

	Output = ""
	for i in xrange(0, len(Bits), 8):
		Output += chr(int(Bits[i:i+8], 2))

	return Output

def Data_9_to_8(InData):
	#don't feel like doing all the math, make a bit list then reparse it
	Bits = ""
	for i in InData:
		Bits += (("0"*8)+bin(ord(i))[2:])[-8:]

	#now add in enough bits to make sure we can send full bytes
	if len(Bits) % 9:
		Bits += "0"*(9-(len(Bits) % 9))

	Output = ""
	for i in xrange(0, len(Bits), 9):
		#just ignore the first bit of all bytes as our tests
		#are all ascii based
		Output += chr(int(Bits[i+1:i+9], 2))

	return Output

#connect then allow us to transfer data
if len(sys.argv) == 3:
        s = socket.create_connection([sys.argv[1], sys.argv[2]])
else:
        s = socket.create_connection(['127.0.0.1', 4000])

while(1):
	(r,w,e) = select.select([s.fileno(), 0], [], [])
	if len(r) == 0:
		break

	#read from one and send to the other
	try:
		if r[0] == s.fileno():
			Data = s.recv(4096)
			if len(Data) == 0:
				break;
			Data = Data_9_to_8(Data)
			sys.stdout.write(Data)
			sys.stdout.flush()
		else:
			Data = sys.stdin.readline()
			Data = Data_8_to_9(Data)
			s.send(Data)
	except Exception as ex:
		print ex
		break
