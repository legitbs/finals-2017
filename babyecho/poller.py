#!/usr/bin/python 

import sys, os
import socket
import select
import re
import time
import random
import string

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

	Output = ""
	i = 0
	bytecount = 0
	while i < len(Bits):
		if i+9 > len(Bits):
			break;

		c = int(Bits[i+1:i+9], 2)
		bytecount += 1
		Output += chr(c)
		if c == 0xa:
			bitstoeat = 8 - (((bytecount*9)+8) % 8)
			i += bitstoeat
			bytecount = 0
		i += 9

	return Output

# simple echo test
def Test_Echo(s):
	# send the string
	out = ''.join(random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase) for _ in range (random.randint(0,30)))
	out9 = Data_8_to_9(out+"\n")
	s.send(out9)

	# read it back
	time.sleep(0.2)
	data9 = s.recv(4096)
	data8 = Data_9_to_8(data9)

	match = ''
	m = re.search("^([a-zA-Z0-9]+).*Reading", data8, re.DOTALL)
	if m:
		match = m.group(1)
		
	if match != out:
		print "Not ok |{}| != |{}|".format(match, out)
		return -1
	
	#print "Ok {} == {}".format(match, out)
	print "Ok"
	return 0

def Test_Fmtstr(s):
	out = ''
	regexp = '^'
	for _ in range(1,random.randint(0,30)):
		# pick a format specifier
		choice = random.randint(0,3)
		if choice == 0:
			# %x
			if random.randint(0,1):
				# include width
				width = random.randint(1,10)
				out += "%0"+str(width)+"x"
				regexp += '([a-f0-9]{{{},}})'.format(width)
			else:
				# don't include width
				out += "%x"
				regexp += '([a-f0-9]+)'

		elif choice == 1:
			# %p
			if random.randint(0,1):
				# include width
				width = random.randint(1,10)
				out += "%0"+str(width)+"p"
				regexp += '([a-f0-9]{{{},}})'.format(width)
			else:
				# don't include width
				out += "%p"
				regexp += '([a-f0-9]+)'

		elif choice == 2:
			# %d
			if random.randint(0,1):
				# include width
				width = random.randint(1,10)
				out += "%0"+str(width)+"d"
				regexp += '([0-9-]{{{},}})'.format(width)
			else:
				# don't include width
				out += "%d"
				regexp += '([0-9-]+)'

		elif choice == 3:
			# %u
			if random.randint(0,1):
				# include width
				width = random.randint(1,10)
				out += "%0"+str(width)+"u"
				regexp += '([0-9]{{{},}})'.format(width)
			else:
				# don't include width
				out += "%u"
				regexp += '([0-9]+)'

		# pick a delimeter
		delimeter = random.choice([".","|","#",",","<",">",'='])
		out += delimeter
		regexp += delimeter

	# look for the newline and Reading prompt to terminate the response
	regexp += ".*Reading"

	# send the string
	out9 = Data_8_to_9(out+"\n")
	s.send(out9)

	# read it back
	time.sleep(0.2)
	data9 = s.recv(4096)
	data8 = Data_9_to_8(data9)

	match = ''
	m = re.search(regexp, data8, re.DOTALL)
	if m:
		#print "Ok {} == {}, {}".format(data8, out, regexp)
		print "Ok"
		return 0
		
	print "Not ok {} == {}, {}".format(data8, out, regexp)
	return -1

if len(sys.argv) == 3:
	s = socket.create_connection([sys.argv[1], sys.argv[2]])
else:
	s = socket.create_connection(['127.0.0.1', 4000])

# read the banner
time.sleep(0.2)
data9 = s.recv(4096)
data8 = Data_9_to_8(data9)
m = re.search("Reading", data8)
if m is None:
	sys.exit(-1)

# Pick a test to run and send it
NumRounds = random.randint(1,30)
for Round in range(1,NumRounds):
	choice = random.randint(0,1)
	print "Round {} of {} choice".format(Round, NumRounds, choice),
	if choice == 0:
		print "Test_Echo result",
		retval = Test_Echo(s)
		if retval:
			sys.exit(retval)
	elif choice == 1:
		print "Test_Fmtstr result",
		retval = Test_Fmtstr(s)
		if retval:
			sys.exit(retval)

sys.exit(0)
