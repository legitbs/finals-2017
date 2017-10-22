from socket import *
from sys import *
from random import *
import telnetlib
from math import *

def ru(s, u):
    z = ''

    while z.endswith(u) == False:
        z += s.recv(1)

    return z

def rl(s):
    z = ''

    while z.endswith('\n') == False:
        z += s.recv(1)

    return z

def eatprompt(s):
    ru(s, '> ')

def pushint(s, stack, a ):
    stack.append(a)
    s.send('{0}\n'.format(a))
    eatprompt(s)
    return

def printstack( s, stack):
    if len(stack) == 0:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    s.send('ps\n')

    y = rl(s)

    ## remove the newline
    z = y.split(' ')[:-1]

    print z

    ## temporarily reverse the stack
    stack.reverse()

    for x in xrange(len(z)):
        if int(z[x]) != stack[x]:
            print '%d %d' %(int(z[x]), stack[x])
            print '[ERROR] Invalid stack values.'
            print '\tExpected: {0}\n\tReceived: {1}'.format(' '.join('{0}'.format(x) for x in stack), y)
            exit(1)

    stack.reverse()
    print '[INFO] printstack passed'
    eatprompt(s)


def add(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(b+c)
    s.send('+\n')
    eatprompt(s)

def sub(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(b-c)
    s.send('-\n')
    eatprompt(s)

def mul(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(b*c)
    s.send('*\n')
    eatprompt(s)

def div(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    s.send('/\n')

    if c == 0:
        print rl(s)
    else:
        ### fix for a python div quirk
        y = float(b)/float(c)

        if y < 0:
            y = int(ceil(y))
        else:
            y = int(floor(y))

        stack.append(y)

    
    eatprompt(s)

def less(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(int(b < c))
    s.send('<\n')
    eatprompt(s)

def lessequal(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(int(b <= c))
    s.send('<=\n')
    eatprompt(s)

def greater(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(int(b > c))
    s.send('>\n')
    eatprompt(s)

def greaterequal(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(int(b >= c))
    s.send('>=\n')
    eatprompt(s)

def equal(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(int(b == c))
    s.send('==\n')
    eatprompt(s)

def dup(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    stack.append(b)
    stack.append(b)

    s.send('dup\n')
    eatprompt(s)

def swap(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()
    c = stack.pop()

    stack.append(b)
    stack.append(c)

    s.send('swap\n')

    eatprompt(s)

def drop(s, stack):
    if len(stack) < 2:
        pushint(s, stack, randint(-100, 100))
        pushint(s, stack, randint(-100, 100))

    b = stack.pop()

    s.send('drop\n')
    eatprompt(s)

def quit(s):
    s.send('quit\n')
    s.close()
    exit()

def main():
    if len(argv) != 3:
        print '{0} <ip> <port>'.format(argv[0])
        exit(1)

    try:
        s = socket(AF_INET, SOCK_STREAM)
        s.connect( (argv[1], int(argv[2])))
    except:
        print '[ERROR] failed to connect to {0}:{1}'.format(argv[1], argv[2])
        exit(1)

    print '[INFO] Connected to {0}:{1}'.format(argv[1], argv[2])

    eatprompt(s)

    stack = []

    try:
        for i in xrange(10):
            pushint(s, stack, randint(-100, 100))
    except:
        print '[ERROR] Failed in initial stack pushing'
        exit(1)

    f = {'drop':drop, 'swap':swap, 'dup':dup, 'add':add, 'sub':sub, 'mul':mul, 'div':div, 'less':less, 'lessequal':lessequal, 'greater':greater, 'greaterequal':greaterequal, 'equal':equal}
    for _ in xrange(30):
        z = choice(f.keys())
        y = f[z]
        print z
        y(s, stack)
        printstack(s, stack)

    quit(s)

main()