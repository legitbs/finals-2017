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

HTTP_DATE_STRING = "Sun, 10 Jul 2017 11:12:30 GMT"
HTTP_SERVER_STRING = "defcon1"

HTTP_MAX_URL = 512

PHP_PAGE_UPLOAD = '''<html>
 <head>
  <title>Upload File</title>
 </head>
 <body>
 [[]] 
 </body>
</html>'''

HTTP_PAGE_TEST = '''<html>
 <head>
  <title>My first webserver</title>
 </head>
 <body>
 Hello World -- This is my first webserver
 </body>
</html>'''

HTTP_PAGE_INDEX = '''<html>
 <head>
 <title>LegitBS Web Server</title>
 <meta http-equiv="refresh" content="1;url=index.php">
 </head>
</html>'''

HTTP_PAGE_TABLE = { 
'/test.htm' : HTTP_PAGE_TEST,
'/index.htm' : HTTP_PAGE_INDEX
}					

def GetRandomWhitespace( ):
	whitespace_string = ""
	for _ in range(random.randint(1, 5)):
		whitespace_string += random.choice( [" ", "\t"] )

	# DEBUG
	whitespace_string = " "
	return whitespace_string	

def Normalize27Bit( x ):
	return (x & 0x7FFFFFF)

def Get27BitNumber( x ):
	if ( x & 0x4000000 ) != 0:
		return -((x ^ 0x7FFFFFF) + 1)
	else:
		return x	

def Data_8_to_9_Bits_FlushNewline(InData, Bits=""):
	for i in InData:
		Bits += (("0"*9)+bin(ord(i))[2:])[-9:]

		if ord(i) == 0xa:
			print "Flushing newline"
			if len(Bits) % 8:
				Bits += "0"*(8-(len(Bits) % 8))
			
	return Bits


def Data_8_to_9_Bits(InData, Bits=""):
	for i in InData:
		Bits += (("0"*9)+bin(ord(i))[2:])[-9:]

	return Bits

def Append_9Bit_List_Bits( InData, InBits ):
	for i in InData:
		InBits += (("0"*9)+bin(i)[2:])[-9:] 

	return InBits

def Convert_9Bit_Bytes( Bits ):
	#add in enough bits to make sure we can send full bytes
	if len(Bits) % 8:
		Bits += "0"*(8-(len(Bits) % 8))

	Output = ""
	for i in xrange(0, len(Bits), 8):
		Output += chr(int(Bits[i:i+8], 2))

	return Output


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

def GetPageData( http_page ):
	return HTTP_PAGE_TABLE[http_page]

	return page_data

def rand_url_var( maxlen ):
	return ''.join( random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase) for _ in range( random.randint(1,maxlen)) )

def rand_url_getvar( maxlen ):
	return ''.join( random.choice(string.ascii_uppercase + string.ascii_lowercase) for _ in range( random.randint(1,maxlen)) )

def rand_url_value( maxlen ):
	return ''.join( random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase + "!@#$%^&*()_+-=`;:'") for _ in range( random.randint(1, maxlen)) )

def GetRandomHTMPage( ):
	return random.choice( HTTP_PAGE_TABLE.keys() )

def GetNextLine( s, recv_line ):
	if len(recv_line) == 0:
		time.sleep( 0.2 )
		data9 = s.recv( 4096 )
		data8 = Data_9_to_8( data9 )

		lines = data8.rstrip('\n').split('\n')

		print "GOT: %s\n" % data8
		print "Lines: ", lines

		recv_line = lines[1:]

		return lines[0].rstrip('\r'), recv_line

	else:
		return_line = recv_line[0]
		
		recv_line = recv_line[1:]

		return return_line.rstrip('\r'), recv_line

def CheckResponse( s, expect_data ):
	recv_data = ''

	recv_line = []

	for line in expect_data.split('\n'):

		compare_line, recv_line = GetNextLine( s, recv_line )
		#print "EXPECT: %s\n" % line

		recv_data += compare_line

		#print "RX: %s\n" % compare_line

		if line != compare_line:
			print "RX[%s] != EXPECT[%s]\n" % (compare_line, line)
			return (recv_data, -1)

	return (recv_data, 0)	

def Test_HTTP_GET(s):
	print "Test_HTTP_GET::"

	get_request = ''
	http_version = random.randint(0,1)
	get_method = "HTTP/1.%d" % http_version

	http_url = GetRandomHTMPage()

	url_get_string = { }
	for _ in range(random.randint(0,5)):
		url_get_string.update( { rand_url_getvar( 10 ) :  rand_url_value( 40 ) } )

	if len( http_url + "?" + urllib.urlencode( url_get_string )) >= HTTP_MAX_URL:
		url_get_string = { }

	http_request = "GET " + http_url + "?" + urllib.urlencode( url_get_string ) + " " + get_method + "\n\n"

	print "DEBUG: GET REQUEST: ", http_request

	s.send( Data_8_to_9( http_request ) )
	
	time.sleep( 1 )

	# Find page
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_data = GetPageData( http_url )

	if ( page_data == None ):
		return -1

	page_response += page_data

	got_response, match_code = CheckResponse( s, page_response ) 

	if ( match_code == -1 ):
		print "DEBUG: GET REQUEST: Data did not match!"

		print "Expected: ", page_response
		print "Got: ", got_response
		
		return -1	

	return 0

def Test_HTTP_POST( s ):
	print "Test_HTTP_POST::"

	post_request = ""
	http_version = random.randint(0,1)
	post_method = "HTTP/1.%d" % http_version

	return 0

def Test_PHP( s ):
	print "Test_PHP"

	# This function calls one of the /test/*.php pages for testing behavior

	test_page_num = random.randint( 0, 3 )

	#DEBUG REMOVE:
	test_page_num = 0

	http_version = random.randint( 0, 1 )

	http_request = ""
	page_response = ""
	
	if test_page_num == 0:
		# Test GET
		a_value = rand_url_value( 100 )
		b_value = rand_url_value( 100 )

		http_request = "GET /test/get.php?%s HTTP/1.%d\n\n" % (urllib.urlencode( { 'A' : a_value, 'B' : b_value } ), http_version)

		print "Test_PHP: GET REQUEST: %s\n" % http_request

		page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
		page_response += "<html>\n <head>\n  <title>Test GET</title>\n </head>\n <body>\n A,B = %s, %s\n </body>\n</html>" % (a_value, b_value)

	elif test_page_num == 1:
		# TODO: Finish
		# Test POST
		a_value = rand_url_value( 126 )
		b_value = rand_url_value( 126 )
		c_value = rand_url_value( 126 )
		c_name = rand_url_var( 126 )

		#post_string = urllib.urlencode( { 'A' : a_value
		#http_request = "POST /test/post.php HTTP/1.%d\nContent-Type: application/\n" % (urllib.urlencode( { 'A' : a_value, 'B' : b_value } ), http_version)
		

	s.send( Data_8_to_9( http_request ) )
	
	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response ) 

	if ( match_code == -1 ):
		print "DEBUG: Test_PHP REQUEST: Data did not match!"

		print "Expected: ", page_response
		print "Got: ", got_response
	
		return -1

	return 0

def DoSendUploadInlineFile_9bit( s, filename, file_data ):
	token_value = 0x1337

	get_string = urllib.urlencode( { 'inline' : 'yes', 'token' : "%d" % token_value } )

	http_version = random.randint( 0, 1 )

	boundary_string = rand_url_var( 20 )

	http_request = "GET /upload.php?%s HTTP/1.%d\nContent-Disposition: form-data; filename=\"%s\";\nContent-Type: multipart/form-data; boundary=\"%s\";\n" % (get_string, http_version, filename, boundary_string)

	http_request += "----%s\n" % boundary_string
	http_request_9bit_part = Append_9Bit_List_Bits( file_data, Data_8_to_9_Bits( http_request ) )
	http_request_9bit = Data_8_to_9_Bits( "\n----%s\n\n" % boundary_string, http_request_9bit_part )

	send_data = Convert_9Bit_Bytes( http_request_9bit )
	# DEBUG::
	#print "UPLOAD REQUEST (len=%d): %s\n" % (len(send_data), Data_9_to_8( send_data ))
 
	s.send( send_data )

	# Check Upload Success
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n Upload Success: %s\n\n </body>\n</html>" % filename
 
	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: DoSendUploadInlineFile_9bit: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response
	
		return -1

	return 0

def TestSendInlineFile_9bit( s, filename, file_data ):
	if DoSendUploadInlineFile_9bit( s, filename, file_data ) == -1:
		return -1

	print "TestSendInlineFile_9bit:: Receiving response (file_data=%d)" % len(file_data)

	# =================================
	http_version = random.randint( 0, 1 )

	http_request = "GET /upload/%s.htm HTTP/1.%d\n\n" % (filename, http_version)

	print "HTTP INLINE FILE REQUEST: %s\n" % http_request

	page_response = "HTTP/1.%d 200 OK\r\nDate: %s\r\nServer: %s\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	page_response_9bit = Append_9Bit_List_Bits( file_data, Data_8_to_9_Bits_FlushNewline( page_response )  )
	#http_request_9bit_part = Append_9Bit_List_Bits( file_data, Data_8_to_9_Bits( http_request ) )

	expected_receive_data = Convert_9Bit_Bytes( Data_8_to_9_Bits( "\r\n", page_response_9bit ) )
	#expected_receive_data = Convert_9Bit_Bytes( page_response_9bit ) + Data_8_to_9( "\r\n" )

	print "RESPONSE BITS: %d\n" % len(page_response_9bit)

	rx_byte_count = 0
	rx_byte_lastpos = 0
	
	while rx_byte_lastpos < len(expected_receive_data):
		rx_data = s.recv( len(expected_receive_data) - rx_byte_lastpos )
		
		rx_byte_count = len(rx_data)

		print "[%d,%d,%d]" % (rx_byte_lastpos, rx_byte_count, len(expected_receive_data))

		receive_chunk = expected_receive_data[rx_byte_lastpos:rx_byte_lastpos+rx_byte_count]

		if ( rx_data != receive_chunk ):
			print "RX Data: %s\n" % binascii.hexlify( rx_data )
			print "EX Data: %s\n" % binascii.hexlify( receive_chunk )
			print "Didn't match! (%d,%d)" % (len(rx_data), len(expected_receive_data))

			return -1

		rx_byte_lastpos += rx_byte_count

		print "MATCH"
		

	print "9-bit data all match!"

	return 0


def DoSendUploadInlineFile( s, filename, file_data ):

	token_value = 0x1337

	get_string = urllib.urlencode( { 'inline' : 'yes', 'token' : "%d" % token_value } )

	http_version = random.randint( 0, 1 )

	boundary_string = rand_url_var( 40 )

	http_request = "GET /upload.php?%s HTTP/1.%d\nContent-Disposition: form-data; filename=\"%s\";\nContent-Type: multipart/form-data; boundary=\"%s\";\n" % (get_string, http_version, filename, boundary_string)

	http_request += "----%s\n" % boundary_string
	http_request += file_data
	http_request += "\n----%s\n\n" % boundary_string

	print "UPLOAD REQUEST: %s\n" % http_request

	# Check Upload Success
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n Upload Success: %s\n\n </body>\n</html>" % filename

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: DoSendUploadInlineFile: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response
	
		return -1

	return 0	

def TestSendInlineFile( s, filename, orig_filedata, verify_filedata ):
	if DoSendUploadInlineFile( s, filename, orig_filedata ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	http_request = "GET /upload/%s.php HTTP/1.%d\n\n" % (filename, http_version)

	print "HTTP INLINE FILE REQUEST: %s\n" % http_request

	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "%s" % verify_filedata

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_MATH: Data did not match!"
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0
	

def DoPostUploadFile( s, file_data ):
	post_string = urllib.urlencode( { 'user' : 'legitbs_poller_user', 'pass' : 'legitbs_poll_pass', 'upload_filedata' : file_data } )

	http_version = random.randint( 0, 1 )

	http_request = "POST /upload.php HTTP/1.%d\nContent-Type: application/x-www-form-urlencoded\nContent-Length: %d\n\n%s\n" % (http_version, len(post_string), post_string)

	print "UPLOAD REQUEST: %s\n" % http_request

	# Check Upload Success
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n Upload success\n </body>\n</html>"

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: DoPostUploadFile: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response
	
		return -1

	return 0	


def Test_PHP_Upload_GET( s ):
	http_request = ""
	page_response = ""
	
	val_count = random.randint( 1, 5 )
	
	get_values = { }	
	for _ in range(val_count):
		get_values.update( { rand_url_getvar( 20 ) : rand_url_value( 60 ) } )

	php_string = ""
	for item, item_value in get_values.iteritems():
		php_string += "echo \"%s\"; echo \" = \"; echo $_GET[\"%s\"]; echo \"<BR>\"; " % (item, item)
		
	file_data = "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n <?php %s ?>\n </body>\n</html>" % php_string

	print "Test_PHP_Upload_GET: Sent File: %s\n" % file_data

	if DoPostUploadFile( s, file_data ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	get_string = urllib.urlencode( get_values )
	http_request = "GET /upload/user.php?%s HTTP/1.%d\n\n" % (get_string, http_version)

	print "HTTP_REQUEST: %s\n" % http_request

	get_string = ""
	for item, item_value in get_values.iteritems():
		get_string += "%s = %s<BR>" % (item, item_value)
	
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n %s\n </body>\n</html>" % get_string

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_POST: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0



def Test_PHP_Upload_POST( s ):
	http_request = ""
	page_response = ""
	
	val_count = random.randint( 1, 5 )
	
	post_values = { }	
	for _ in range(val_count):
		post_values.update( { rand_url_var( 40 ) : rand_url_value( 120 ) } )

	php_string = ""
	for item, item_value in post_values.iteritems():
		php_string += "echo \"%s\"; echo \" = \"; echo $_POST[\"%s\"]; echo \"<BR>\"; " % (item, item)
		
	file_data = "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n <?php %s ?>\n </body>\n</html>" % php_string

	print "Test_PHP_Upload_Post: Sent File: %s\n" % file_data

	if DoPostUploadFile( s, file_data ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	post_string = urllib.urlencode( post_values )
	http_request = "POST /upload/user.php HTTP/1.%d\nContent-Type: application/x-www-form-urlencoded\nContent-Length: %d\n\n%s\n" % (http_version, len(post_string), post_string)

	post_string = ""
	for item, item_value in post_values.iteritems():
		post_string += "%s = %s<BR>" % (item, item_value)
	
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n %s\n </body>\n</html>" % post_string

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_POST: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0


def Test_PHP_Upload_COOKIE( s ):
	http_request = ""
	page_response = ""
	
	val_count = random.randint( 1, 5 )
	
	cookie_values = { }	
	for _ in range(val_count):
		cookie_values.update( { rand_url_var( 40 ) : rand_url_value( 60 ) } )

	php_string = ""
	for item, item_value in cookie_values.iteritems():
		php_string += "echo \"%s\"; echo \" = \"; echo $_COOKIE[\"%s\"]; echo \"<BR>\"; " % (item, item)
		
	file_data = "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n <?php %s ?>\n </body>\n</html>" % php_string

	print "Test_PHP_Upload_COOKIE: Sent File: %s\n" % file_data

	if DoPostUploadFile( s, file_data ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	cookie_string = ""
	for item, item_value in cookie_values.iteritems():
		if len(cookie_string) > 0:
			cookie_string += "; "

		cookie_string += "%s=%s" % (item, urllib.quote(item_value))

	http_request = "GET /upload/user.php HTTP/1.%d\nCookie: %s\n\n" % (http_version, cookie_string)

	print "HTTP COOKIE REQUEST: %s\n" % http_request

	cookie_string = ""
	for item, item_value in cookie_values.iteritems():
		cookie_string += "%s = %s<BR>" % (item, item_value)
	
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n %s\n </body>\n</html>" % cookie_string

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_COOKIE: Data did not match!"
	
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0


def Test_PHP_Upload_SETCOOKIE( s ):
	http_request = ""
	page_response = ""
	
	val_count = random.randint( 1, 5 )
	
	cookie_values = { }	
	for _ in range(val_count):
		cookie_values.update( { rand_url_getvar( 10 ) : rand_url_value( 60 ) } )

	from_type = random.randint( 0, 2 )
	from_string = ""
	if from_type == 0:
		from_string = "GET"
	elif from_type == 1:
		from_string = "POST"
	elif from_type == 2:
		from_string = "COOKIE"
	else:
		from_string = "GET"	

	php_string = ""
	for item, item_value in cookie_values.iteritems():
		php_string += "setcookie( \"%s\", $_%s[\"%s\"] ); echo \"%s\"; echo \" = \"; echo $_%s[\"%s\"]; echo \"<BR>\";\n" % (item, from_string, item, item, from_string, item)
		#php_string += "echo \"%s\"; echo \" = \"; echo $_%s[\"%s\"]; echo \"<BR>\";\n" % (item, from_string, item)
		
	file_data = "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n <?php %s ?>\n </body>\n</html>" % php_string

	print "Test_PHP_Upload_SETCOOKIE: Sent File: %s\n" % file_data

	if DoPostUploadFile( s, file_data ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	url_add_string = ""	
	if from_type == 0 or from_type == 2:
		# GET or GET with COOKIE
		request_type = "GET"
	else:
		request_type = "POST"

	if from_type == 0:
		# GET (url)
		url_add_string = "?" + urllib.urlencode( cookie_values )

	http_request = "%s /upload/user.php%s HTTP/1.%d\n" % (request_type, url_add_string, http_version)

	if from_type == 0:
		print "GET request"
		http_request += "\n"
	elif from_type == 1:
		print "POST request"
		post_string = urllib.urlencode( cookie_values )

		http_request += "Content-Type: application/x-www-form-urlencoded\nContent-Length: %d\n\n%s\n" % (len(post_string), post_string)
	elif from_type == 2:
		print "COOKIE request"
		cookie_string = ""
		for item, item_value in cookie_values.iteritems():
			if len(cookie_string) > 0:
				cookie_string += "; "

			cookie_string += "%s=%s" % (item, urllib.quote(item_value))

		http_request += "Cookie: %s\n\n" % (cookie_string)
		


	cookie_string = ""
	for item, item_value in cookie_values.iteritems():
		cookie_string += "%s = %s<BR>" % (item, item_value)
	
	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)

	for item, item_value in cookie_values.iteritems():
		page_response += "Set-Cookie: %s=%s; path=/;\n" % (item, item_value)	

	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n %s\n </body>\n</html>" % cookie_string

	print "HTTP SETCOOKIE REQUEST: %s\n" % http_request

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_COOKIE: Data did not match!"
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0
	

def Test_PHP_Upload_MATH( s ):
	print "Test_PHP_Upload_MATH::"
	http_request = ""
	page_response = ""
	
	math_operation_count = random.randint( 1, 10 )
	math_operand_count = random.randint( 2, 4 )

	math_operands = []
	operand_values = []

	php_string = ""
	for _ in range(math_operand_count):
		operand_name = rand_url_getvar( 10 )
		operand_value = random.randint( 0, 0x7FFFFFF )
		math_operands.append( operand_name )
		operand_values.append( operand_value )
		
		php_string += "%s$%s%s=%s%d;%s\n" % (GetRandomWhitespace(), operand_name, GetRandomWhitespace(), GetRandomWhitespace(), operand_value, GetRandomWhitespace())

	php_string += "\n"	

	syntax_error = False
	try:
		for _ in range(math_operation_count):
			#operator = random.choice( ["+", "-", "/", "*", "%"] )
			operator = random.choice( ["+", "-", "*"] )

			# 1 = 2 <opr> 3
			operand1_idx = random.randint( 0, len(math_operands)-1 )
			operand2_idx = random.randint( 0, len(math_operands)-1 )
			operand3_idx = random.randint( 0, len(math_operands)-1 )

			operand1 = math_operands[operand1_idx]
			operand2 = math_operands[operand2_idx]
			operand3 = math_operands[operand3_idx]

			php_string += "%s%s%s=%s%s%s%s%s%s%s;%s\n" % (GetRandomWhitespace(), "$" + operand1, GetRandomWhitespace(), GetRandomWhitespace(), "$" + operand2, GetRandomWhitespace(), operator, GetRandomWhitespace(), "$" + operand3, GetRandomWhitespace(), GetRandomWhitespace())	

			if operator == "+":
				operand_values[operand1_idx] = Normalize27Bit(operand_values[operand2_idx] + operand_values[operand3_idx])
			elif operator == "-":
				operand_values[operand1_idx] = Normalize27Bit(operand_values[operand2_idx] - operand_values[operand3_idx])
			elif operator == "/":
				operand_values[operand1_idx] = Normalize27Bit(operand_values[operand2_idx] / operand_values[operand3_idx])
			elif operator == "*":
				operand_values[operand1_idx] = Normalize27Bit(operand_values[operand2_idx] * operand_values[operand3_idx])
			elif operator == "%":
				operand_values[operand1_idx] = Normalize27Bit(operand_values[operand2_idx] % operand_values[operand3_idx])

	except ZeroDivisionError:
		syntax_error = True

	math_result_string = ""
	for idx in range( math_operand_count ):
		php_string += "echo \"%s\"; echo \" = \"; echo $%s; echo \"<BR>\n\";\n" % (math_operands[idx], math_operands[idx])
		
		math_result_string += "%s = %d<BR>\n" % (math_operands[idx], Get27BitNumber(operand_values[idx]))
		
	file_data = "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n <?php %s ?>\n </body>\n</html>" % php_string

	print "Test_PHP_Upload_MATH: Sent File: %s\n" % file_data

	if DoPostUploadFile( s, file_data ) == -1:
		return -1

	http_version = random.randint( 0, 1 )

	http_request = "GET /upload/user.php HTTP/1.%d\n\n" % (http_version)

	print "HTTP MATH REQUEST: %s\n" % http_request

	page_response = "HTTP/1.%d 200 OK\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<html>\n <head>\n  <title>Upload File</title>\n </head>\n <body>\n %s\n </body>\n</html>" % math_result_string

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_PHP_Upload_MATH: Data did not match!"
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0
	

def Test_PHP_Upload( s ):
	print "Test_PHP_Upload"

	upload_test_num = random.randint( 0, 4 )

	# DEBUG force to 1 for testing
	#upload_test_num = 4

	if upload_test_num == 0:
		return Test_PHP_Upload_POST( s )
	elif upload_test_num == 1:
		return Test_PHP_Upload_COOKIE( s )
	elif upload_test_num == 2:
		return Test_PHP_Upload_GET( s )
	elif upload_test_num == 3:
		return Test_PHP_Upload_SETCOOKIE( s )
	elif upload_test_num == 4:
		return Test_PHP_Upload_MATH( s )

	# This function uploads a file and
	return 0

def Test_Fail( s ):
	print "Test_Fail(): "

	# Request a non-existent page
	bad_url = ''.join( random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase + "!@#$%^&*()_+-=`;:'") for _ in range( random.randint(1, 500)) )

	http_extension = random.choice( ["htm", "php", "gif", "jpg", "html"] )
	http_version = random.randint( 0, 1 )

	http_request = "GET /%s.%s HTTP/1.%d\n\n" % (bad_url, http_extension, http_version)

	
	#page_response = "HTTP/1.%d 404 Not Found\n" % (http_version)
	page_response = "HTTP/1.%d 404 Not Found\nDate: %s\nServer: %s\nConnection: close\nContent-Type: text/html\n\n" % (http_version, HTTP_DATE_STRING, HTTP_SERVER_STRING)
	page_response += "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD><BODY>404 Not Found</BODY></HTML>"

	print "Test_Fail REQUEST: %s\n" % http_request

	s.send( Data_8_to_9( http_request ) )

	time.sleep( 1 )

	got_response, match_code = CheckResponse( s, page_response )

	if ( match_code == -1 ):
		print "ERROR: Test_Fail: Data did not match!"
		print "Expected: ", page_response
		print "Got: ", got_response

		return -1

	return 0
	


if len(sys.argv) == 3:
	s = socket.create_connection(['10.5.' + sys.argv[1] + '.2', sys.argv[2]])
else:
	s = socket.create_connection(['127.0.0.1', 4000])

rand_seed = binascii.hexlify( os.urandom( 20 ) )

# DEBUG
#rand_seed = "311ee4558269f4483310899b89e6f87c5253f88e"
#rand_seed = "1e7a2850b551abf13de7f53c6ee088189b55a5ca"
#rand_seed = "35693b6fbcfd0173eb6d2b7bd43c8d8dd80e4dd1"
random.seed( rand_seed )

print "[POLLER CONNECTED] RAND STATE: ", rand_seed

#time.sleep( 5 )

# Send request
# Test Count
test_count = random.randint(1, 7)

for test_number in range(0, test_count):
	print "Round: %d\n" % test_number

	test_type = random.randint(0,6)

	# TODO: Remove after debugging
	#test_type = 5

	if test_type == 0:
		result = Test_HTTP_GET(s)
	elif test_type == 1:
		result = Test_HTTP_POST(s)
	elif test_type == 2:
		result = Test_PHP( s )
	elif test_type == 3 or test_type == 4:
		result = Test_PHP_Upload( s )
	elif test_type == 5:
		filedata_9bit = []
		for _ in xrange(random.randint(40, 600)):
			new_val = random.randint( 0, 0x1FF )
			while new_val == 0xA:
				new_val = random.randint( 0, 0x1FF )

			filedata_9bit.append( new_val )

		filename = rand_url_var( 20 )

		result = TestSendInlineFile_9bit( s, filename, filedata_9bit )
	else:
		result = Test_Fail( s ) 

	if ( result != 0 ):
		sys.exit( -1 )

	time.sleep( 0.4 )

sys.exit(0)
