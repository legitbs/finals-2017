#!/usr/bin/python 

import sys, os
import socket
import select
import random
import math
import string
import struct
import binascii
import time

selffuncs = []
addrstack = []

def Normalize27Bit( x ):
   return (x & 0x7FFFFFF)

def Get27BitNumber( x ):
   if ( x & 0x4000000 ) != 0:
      return -((x ^ 0x7FFFFFF) + 1)
   else:
      return x

def WRAP27( x ):
	x = (x & 0x7FFFFFF)
   
	if ( x & 0x4000000 ) != 0:
		return -((x ^ 0x7FFFFFF) + 1)
	else:
		return x
	

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

def ru(s, u):
    z = ''
    y = ''

    while y.endswith(u) == False:
        z += s.recv(1)
        y = Data_9_to_8(z)

    return y.rstrip('\x00')

def send( s, data ):
	time.sleep( 0.1 )
	z = Data_8_to_9( data )

	s.send( z )

def read( s, l ):
	z = Data_9_to_8(s.recv(l))

	return z

def rl(s):
    z = ''

    while z.endswith('\n') == False:
        z += s.recv(1)

    return z

def eatprompt(s):
    return Data_9_to_8(s.recv(4096))


def pushint(s, stack, a ):
	print '[INFO] Pushing integer: %d' %(a)

	stack.append(a)

	if random.choice([True, False]):
		send( s, '{0}\n'.format(a))
	else:
		if a < 0:
			a *= -1
			stack[-1] *= -1

		send(s, '0x%x\n' %(a))

	y = eatprompt(s)

	return y

def printstack( s, stack):
	print '[INFO] Printing the stack'

	if len(stack) == 0:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	send(s, 'ps\n')

	y = read(s, 4096).rstrip()

	print '--------'
	print y
	print '--------'

	## remove the newline
	z = y.split(' ')[:-1]

	## temporarily reverse the stack
	stack.reverse()

	for x in xrange(len(z)):
		if int(z[x]) != stack[x]:
			print '%d %d' %(int(z[x]), stack[x])
			print '[ERROR] Invalid stack values.'
			print '\tExpected: {0}\n\tReceived: {1}'.format(' '.join('{0}'.format(x) for x in stack), y)
			s.close()
			exit(1)

	stack.reverse()
	print '[INFO] printstack passed'
	eatprompt(s)

def swap(s, stack):
	print '[INFO] Testing swap'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(b)
	stack.append(c)

	send(s, 'swap\n')
	eatprompt(s)

def add(s, stack):
	print '[INFO] Testing add'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = WRAP27(stack.pop())
	c = WRAP27(stack.pop())

	stack.append(WRAP27(b+c))
	send(s, '+\n')
	eatprompt(s)

def sub(s, stack):
	print '[INFO] Testing sub'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = WRAP27(stack.pop())
	c = WRAP27(stack.pop())

	
	stack.append(WRAP27(b-c))

	send(s, '-\n')
	eatprompt(s)

def mul(s, stack):
	print '[INFO] Testing mul'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = WRAP27(stack.pop())
	c = WRAP27(stack.pop())

	stack.append(WRAP27(b*c))
	send(s, '*\n')
	eatprompt(s)

def div(s, stack):
	print '[INFO] Testing div'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = WRAP27(stack.pop())
	c = WRAP27(stack.pop())

	send(s, '/\n')

	if c == 0:
		print read(s, 4096)
	else:
		### fix for a python div quirk
		y = float(b)/float(c)

		if y < 0:
			y = int(math.ceil(y))
		else:
			y = int(math.floor(y))

		stack.append(WRAP27(y))

	eatprompt(s)

def modu(s, stack):
	print '[INFO] Testing mod'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = WRAP27(stack.pop())
	c = WRAP27(stack.pop())

	send(s, '%\n')

	if c == 0:
		print read(s, 4096)
	else:
		y = abs(b) % abs(c)

		if b < 0:
			y *= -1

		stack.append(WRAP27(y))

	eatprompt(s)

def less(s, stack):
	print '[INFO] Testing less'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(int(b < c))
	send(s, '<\n')
	eatprompt(s)

def lessequal(s, stack):
	print '[INFO] Testing lessequal'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(int(b <= c))
	send(s, '<=\n')
	eatprompt(s)

def greater(s, stack):
	print '[INFO] Testing greater'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(int(b > c))
	send(s, '>\n')
	eatprompt(s)

def greaterequal(s, stack):
    print '[INFO] Testing greaterequal'

    if len(stack) < 2:
    	pushint(s, stack, random.randint(-100, 100))
    	pushint(s, stack, random.randint(-100, 100))

    #printstack(s, stack)

    #b = stack.pop()
    #c = stack.pop()

    #stack.append(int(b >= c))
    #print 'Expect: %d' %(int(b >= c ))
    #send(s, '>=\n')
    #eatprompt(s)
    

def equal(s, stack):
	print '[INFO] Testing equal'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(int(b == c))
	send(s, '==\n')
	eatprompt(s)

def duptwo(s, stack):
	print '[INFO] Testing 2dup'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(c)
	stack.append(b)
	stack.append(c)
	stack.append(b)

	send(s, '2dup\n')
	eatprompt(s)

def dup(s, stack):
	print '[INFO] Testing dup'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	stack.append(b)
	stack.append(b)

	send(s, 'dup\n')
	eatprompt(s)

def drop(s, stack):
	print '[INFO] Testing drop'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()

	send(s, 'drop\n')
	eatprompt(s)

def quit(s):
    s.send('quit\n')
    s.close()
    exit()

def interactive(s):
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

def invalid(s, stack):
	print '[INFO] Testing invalid string'
	r = ''

	for _ in range( random.randint(1, 7) ):
		r += random.choice( string.lowercase)

	send(s, r + '\n')
	y = read(s,4096)
	if y.find('[-] invalid: {0}'.format(r)) == -1:
		print '[ERROR] Failed to read invalid string: %s' %(y)
		s.close()
		exit(1)
	eatprompt(s)

def pc( s, stack ):
	print '[INFO] Testing .'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	send( s, '.\n')
	y = read( s, 4096)

	a = stack.pop()

	if (a & 0xff) != (struct.unpack('b', y[0])[0] & 0xff):
		print '[ERROR] putch test failed: %d -- %d' %(a, struct.unpack('b', y[0])[0])
		s.close()
		exit(1)

	if y.find('> ') == -1:
		y = read(4096)

def upa(s, stack):
	print '[INFO] Testing upa'
	send(s, 'db\n')
	y = read(s, 4096)

	y = y.rstrip()
	y = int(y, 16)

	eatprompt(s)

	return y

def rot(s, stack):
	print '[INFO] Testing rot'

	if len(stack) < 3:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	a = stack.pop()
	b = stack.pop()
	c = stack.pop()

	stack.append(b)
	stack.append(a)
	stack.append(c)

	send(s, 'rot\n')
	eatprompt(s)

def null( s, stack):
	print '[INFO] Testing null'
	send(s, 'null\n')
	eatprompt(s)

def newfunc( s, stack):
	global selffuncs

	print '[INFO] Testing new function'

	r = ''

	for _ in xrange(random.randint(4, 7) ):
		r += random.choice(string.lowercase)

	a = random.randint( -100, 100)
	b = random.randint( -100, 100)
	op = random.choice(['+', '-', '*'])
	value = eval(str(a) + op + str(b))

	y = ': {0} {1} {2} {3} ;\n'.format(r, b, a, op)

	send(s, y)
	eatprompt(s)
	send(s, r + '\n')
	stack.append(value)
	eatprompt(s)

	selffuncs.append(r)

def deln( s, stack):
	global selffuncs

	print '[INFO] Testing dd'

	if len(selffuncs) == 0:
		newfunc(s, stack)
		newfunc(s, stack)

	r = random.choice(selffuncs)
	selffuncs.remove(r)

	send( s, 'dd %s\n' %(r))
	eatprompt(s)
	printstack(s, stack);

	send( s, '%s\n' %(r))
	y = read(s, 4096)

	print 'yoloz'

	print y
	if y.find('[-] invalid: %s' %(r)) == -1:
		print '[ERROR] failed to delete function'
		s.close()
		exit(1)

	if y.find('> ') == -1:
		print read(s, 4096)

def pushua( s, stack ):
	print '[INFO] Testing pushua'

	y = upa(s, stack)

	send(s, 'dp\n')
	eatprompt(s)
	send(s, 'dio\n')
	print eatprompt(s)

	y += 3

	print 'a'
	stack.append(y)
	printstack(s, stack)

	d = random.randint(5,10)

	send(s, 'drop\n')
	eatprompt(s)

	stack.pop()
	printstack(s, stack)

	pushint( s, stack, d)

	print 'b'
	printstack(s, stack)
	print 'c'

	pushint(s, stack, y)
	print 'd'
	printstack(s, stack)
	print 'f'
	y += (d*3)

	send(s, 'ad\n')
	eatprompt( s )

	print 'e'
	stack.pop()
	stack.pop()
	stack.append(y)
	printstack(s, stack)

	send(s, 'drop\n')	
	print eatprompt(s)
	print 'f'

	stack.pop()
	printstack(s, stack)
	print 'h'

	d = random.randint(5, 100)
	pushint(s, stack, d)

	print 'i'
	printstack( s, stack )

	print 'g'
	pushint(s, stack, y)

	printstack(s, stack)

	send( s, 'qq\n')
	eatprompt(s)

	stack.pop()
	stack.pop()

	printstack(s, stack)

	pushint(s, stack, y)

	send(s, 'abs\n')
	eatprompt(s)

	stack.pop()
	stack.append(d)

	printstack(s, stack)

	if len(stack) > 5:
		for i in range(4):
			send(s, 'drop\n')
			eatprompt(s)
			stack.pop()
			printstack(s, stack)

def genif( s, stack ):
	print '[INFO] Testing if'

	a = random.choice( [0, random.randint(1, 100) ])

	b = random.randint( -100, 100)
	c = random.randint( -100, 100)
	op = random.choice(['+', '-', '*'])

	d = random.randint( -100, 100)
	e = random.randint( -100, 100)
	fop = random.choice(['+', '-', '*'])

	y = '%d if %d %d %s then %d %d %s endif\n' %(a, b, c, op, d, e, fop)

	send(s, y)
	eatprompt(s)

	if a != 0:
		value = eval(str(c) + op + str(b))
	else:
		value = eval(str(e) + fop + str(d))

	stack.append( value )

def handstr( s, stack ):
	print '[INFO] Testing new string'

	z = ''

	for i in xrange(5, 10):
		z += random.choice( string.lowercase )

	send( s, ".\" " + z + " .\"\n")
	y = read(s, 4096)

	if y.find(z) == -1:
		print '[ERROR] String gen failed'
		s.close()
		exit(1)

	eatprompt(s)

def main(argv):
	if len(argv) != 3:
		print '{0} <ip> <port>'.format(argv[0])
		s.close()
		exit(1)

	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect( (argv[1], int(argv[2])))
	except:
		print '[ERROR] failed to connect to {0}:{1}'.format(argv[1], argv[2])
		s.close()
		exit(1)

	print '[INFO] Connected to {0}:{1}'.format(argv[1], argv[2])

	rand_seed = binascii.hexlify( os.urandom( 20 ) )

	# DEBUG
	#rand_seed = "311ee4558269f4483310899b89e6f87c5253f88e"
	#rand_seed = "1e7a2850b551abf13de7f53c6ee088189b55a5ca"
	#rand_seed = "35693b6fbcfd0173eb6d2b7bd43c8d8dd80e4dd1"
	#rand_seed = "02a8b25f9268319ce32f20a391cf7531d236cf11"
	#rand_seed = "32883b6be48a1b25af7cbfbf285d04bbc7a99943"
	random.seed( rand_seed )

	print "[POLLER CONNECTED] RAND STATE: ", rand_seed

	try:
		eatprompt(s)
	except:
		print '[ERROR] Initial eat prompt failed'
		exit(1)

	stack = []

	try:
		for i in xrange(random.randint(0, 15)):
			pushint( s, stack, random.randint(-100, 100))
	except:
		print '[ERROR] Initial pushes failed'
		s.close()
		exit(1)

	try:
		printstack( s, stack )
	except:
		print '[ERROR] Initial printstack failed'
		s.close()
		exit(1)

	# 'pu': pushua,
	f = { 'pu': pushua, 'dd': deln, 'genif': genif, 'str': handstr,'2dup': duptwo, 'newfunc': newfunc, 'rot': rot, 'null':null, 'upa': upa, 'modu': modu, 'putc': pc, 'drop':drop, 'swap':swap, 'dup':dup, 'add':add, 'sub':sub,'mul':mul, 'div':div, 'less':less, 'lessequal':lessequal, 'greater':greater, 'equal':equal}

	for _ in xrange(random.randint(5, 40)):
		z = random.choice(f.keys())
		y = f[z]
		y(s, stack)
		printstack(s, stack)


	#Sprint 'Interactive...'
	#interactive(s)
	quit(s)

main(sys.argv)
