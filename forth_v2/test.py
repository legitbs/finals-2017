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

a = "bbbbbbbbbbbbbbbbbb"
b = Data_8_to_9(a)
for x in b:
	print hex(ord(x))
