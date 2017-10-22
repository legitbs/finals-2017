#!/usr/bin/python 

import sys, os
import socket
import select
import random
import math
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

	#now add in enough bits to make sure we can send full bytes
	if len(Bits) % 9:
		Bits += "0"*(9-(len(Bits) % 9))

	Output = ""
	for i in xrange(0, len(Bits), 9):
		#just ignore the first bit of all bytes as our tests
		#are all ascii based
		Output += chr(int(Bits[i+1:i+9], 2))

	return Output

def ru(s, u):
    z = ''
    y = ''

    while y.endswith(u) == False:
        z += s.recv(1)
        y = Data_9_to_8(z)

    return y.rstrip('\x00')

def send( s, data ):
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
    stack.append(a)

    if random.choice([True, False]):
    	send( s, '{0}\n'.format(a))
    else:
    	if a < 0:
    		a *= -1
    		stack[-1] *= -1

    	send(s, '0x%x\n' %(a))
    
    y = read(s, 4096)

    return y

def printstack( s, stack):
	print '[INFO] Printing the stack'

	if len(stack) == 0:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	send(s, 'ps\n')

	y = read(s, 4096).rstrip()

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

	b = stack.pop()
	c = stack.pop()

	stack.append(b+c)
	send(s, '+\n')
	eatprompt(s)

def sub(s, stack):
	print '[INFO] Testing sub'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	
	stack.append(b-c)

	send(s, '-\n')
	eatprompt(s)

def mul(s, stack):
	print '[INFO] Testing mul'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

	stack.append(b*c)
	send(s, '*\n')
	eatprompt(s)

def div(s, stack):
	print '[INFO] Testing div'

	if len(stack) < 2:
		pushint(s, stack, random.randint(-100, 100))
		pushint(s, stack, random.randint(-100, 100))

	b = stack.pop()
	c = stack.pop()

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

		stack.append(y)

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
    eatprompt(s)
    

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
		print '[ERROR] initial print failed'
		s.close()
		exit(1)

	f = {'invalid': invalid, 'drop':drop, 'swap':swap, 'dup':dup, 'add':add, 'sub':sub,'mul':mul, 'div':div, 'less':less, 'lessequal':lessequal, 'greater':greater, 'equal':equal}

	for _ in xrange(30):
		z = random.choice(f.keys())
		y = f[z]
		y(s, stack)
		printstack(s, stack)


	#Sprint 'Interactive...'
	#interactive(s)
	quit(s)

main(sys.argv)
