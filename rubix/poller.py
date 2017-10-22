#!/usr/bin/python 

import sys, os
import socket
import select
import re
import time
import random
from cLEMENCy import *
from ctypes import *

OutputBufAddr = 0x14040
OutputCountAddr = 0x14048

Moves = ["U'", "U", "D'", "D", "L'", "L", "R'", "R", "F'", "F", "B'", "B"]

def GenerateSolution(code):

	seed = code[0] << 9
	seed += code[1] << 18
	seed += code[2]

	SeedRand(c_uint(seed))

	Solution = []
	for i in (range(0,30)):
		rnd = GimmeRand()
		Solution.append(Moves[rnd % 12])

	return(' '.join(reversed(Solution)))

def IllegalInstruction():
	Instr = '111000001'

	Hex = hex(int(Instr[0:1], 2))
	Hex += hex(int(Instr[1:5], 2))
	Hex += hex(int(Instr[5:9], 2))

	return(Hex.replace('0x', ''))

def RandomInstruction():
	AvailableRegisters = [ R15, R16, R17, R18, R19, R20, R21, R22, R23, R24, R25, R26, R27, R28 ]

	r = random.randint(0,31)
	rA = random.choice(AvailableRegisters)
	rB = random.choice(AvailableRegisters)
	rC = random.choice(AvailableRegisters)
	Immediate = random.randint(0,0x7f)
	Immediate17 = random.randint(0,0x1ffff)
	if r == 0:
		print("AD({}, {}, {})".format(rA, rB, rC))
		return(AD(rA, rB, rC))
	elif r == 1:
		print("ADC({}, {}, {})".format(rA, rB, rC))
		return(ADC(rA, rB, rC))
	elif r == 2:
		print("ADCI({}, {}, {})".format(rA, rB, Immediate))
		return(ADCI(rA, rB, Immediate))
	elif r == 3:
		print("ADI({}, {}, {})".format(rA, rB, Immediate))
		return(ADI(rA, rB, Immediate))
	elif r == 4:
		print("AN({}, {}, {})".format(rA, rB, rC))
		return(AN(rA, rB, rC))
	elif r == 5:
		print("ANI({}, {}, {})".format(rA, rB, Immediate))
		return(ANI(rA, rB, Immediate))
	elif r == 6:
		print("CM({}, {}, {})".format(rA, rB, rC))
		return(CM(rA, rB))
	elif r == 7:
		print("CMI({}, {}, {})".format(rA, rB, Immediate))
		return(CMI(rA, Immediate))
	elif r == 8:
		print("MH({}, {}, {})".format(rA, rB, Immediate17))
		return(MH(rA, Immediate17))
	elif r == 9:
		print("ML({}, {}, {})".format(rA, rB, Immediate17))
		return(ML(rA, Immediate17))
	elif r == 10:
		print("MS({}, {}, {})".format(rA, rB, Immediate17))
		return(MS(rA, Immediate17))
	elif r == 11:
		print("MU({}, {}, {})".format(rA, rB, rC))
		return(MU(rA, rB, rC))
	elif r == 12:
		print("MUI({}, {}, {})".format(rA, rB, Immediate))
		return(MUI(rA, rB, Immediate))
	elif r == 13:
		print("MUS({}, {}, {})".format(rA, rB, rC))
		return(MUS(rA, rB, rC))
	elif r == 14:
		print("NG({}, {}, {})".format(rA, rB, rC))
		return(NG(rA, rB))
	elif r == 15:
		print("NT({}, {}, {})".format(rA, rB, rC))
		return(NT(rA, rB))
	elif r == 16:
		print("OR({}, {}, {})".format(rA, rB, rC))
		return(OR(rA, rB, rC))
	elif r == 17:
		print("ORI({}, {}, {})".format(rA, rB, Immediate))
		return(ORI(rA, rB, Immediate))
	elif r == 18:
		print("RL({}, {}, {})".format(rA, rB, rC))
		return(RL(rA, rB, rC))
	elif r == 19:
		print("RLI({}, {}, {})".format(rA, rB, Immediate))
		return(RLI(rA, rB, Immediate))
	elif r == 20:
		print("RR({}, {}, {})".format(rA, rB, rC))
		return(RR(rA, rB, rC))
	elif r == 21:
		print("RRI({}, {}, {})".format(rA, rB, Immediate))
		return(RRI(rA, rB, Immediate))
	elif r == 22:
		print("SA({}, {}, {})".format(rA, rB, rC))
		return(SA(rA, rB, rC))
	elif r == 23:
		print("SAI({}, {}, {})".format(rA, rB, Immediate))
		return(SAI(rA, rB, Immediate))
	elif r == 24:
		print("SB({}, {}, {})".format(rA, rB, rC))
		return(SB(rA, rB, rC))
	elif r == 25:
		print("SBI({}, {}, {})".format(rA, rB, Immediate))
		return(SBI(rA, rB, Immediate))
	elif r == 26:
		print("SL({}, {}, {})".format(rA, rB, rC))
		return(SL(rA, rB, rC))
	elif r == 27:
		print("SLI({}, {}, {})".format(rA, rB, Immediate))
		return(SLI(rA, rB, Immediate))
	elif r == 28:
		print("SR({}, {}, {})".format(rA, rB, rC))
		return(SR(rA, rB, rC))
	elif r == 29:
		print("SRI({}, {}, {})".format(rA, rB, Immediate))
		return(SRI(rA, rB, Immediate))
	elif r == 30:
		print("XR({}, {}, {})".format(rA, rB, rC))
		return(XR(rA, rB, rC))
	elif r == 31:
		print("XRI({}, {}, {})".format(rA, rB, Immediate))
		return(XRI(rA, rB, Immediate))

def Test_Echo(s):

	NumChars = random.randint(1,20)
	CharToSend = random.randint(33,126)
	code = []
	OutputCountSet = 0
	AvailableInstructions = 3 

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	if random.randint(0,1) and OutputCountSet == 0:
		print "ML(R02, 0)"
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# mix up the order of sending the OutputBufAddr, CharToSend, and NumChars to send
	rnd = random.randint(0,3)
	if rnd == 0:
		print "ML(R01, 0)"
		code.append(ML(R01, 0))
		print "MH(R01, {})".format(OutputBufAddr)
		code.append(MH(R01, OutputBufAddr))
		print "ML(R08, {})".format(CharToSend)
		code.append(ML(R08, CharToSend))
		print "ML(R03, {})".format(NumChars)
		code.append(ML(R03, NumChars))
	elif rnd == 1:
		code.append(ML(R08, CharToSend))
		code.append(ML(R01, 0))
		code.append(MH(R01, OutputBufAddr))
		code.append(ML(R03, NumChars))
	elif rnd == 2:
		code.append(ML(R01, 0))
		code.append(ML(R08, CharToSend))
		code.append(ML(R03, NumChars))
		code.append(MH(R01, OutputBufAddr))
	elif rnd == 3:
		code.append(ML(R03, NumChars))
		code.append(ML(R01, 0))
		code.append(ML(R08, CharToSend))
		code.append(MH(R01, OutputBufAddr))

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# randomly set the OutputCountAddr register
	if random.randint(0,1) and OutputCountSet == 0:
		print "ML(R02, {})".format(0)
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# create the loop to write the chars to the buffer
	print "STS(R08, R01)"
	code.append(STS(R08, R01))
	print "ADI(R01, R01, 1)"
	code.append(ADI(R01, R01, 1))
	print "SBI(R03, R03, 1)"
	code.append(SBI(R03, R03, 1))
	code.append(B(N, -12))

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# set the OutputCountAddr register if it hasn't been set yet
	if OutputCountSet == 0:
		print "ML(R02, {})".format(0)
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	print "ML(R03, {})".format(NumChars)
	code.append(ML(R03, NumChars))
	print "STT(R03, R02)"
	code.append(STT(R03, R02))

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	print "HT()"
	code.append(HT())

	Sc = MakeSc(code)
	Solution = GenerateSolution(Sc)
	SendSc(s, Sc)
	time.sleep(0.1)

	return ([chr(CharToSend)*NumChars, Solution])

def Test_Math(s):

	code = []
	OutputCountSet = 0
	AvailableInstructions = 0 

	# what are we going to do
	Input1 = random.randint(1,0x7f)
	Input2 = random.randint(1,0x7f)
	Operation = random.randint(0, 3)
	if Operation == 0:
		# add
		Output = Input1 + Input2
		op = '+'
	elif Operation == 1:
		# subtract
		# don't want to deal with negative numbers in the output
		if (Input1 < Input2):
			Temp = Input1
			Input1 = Input2
			Input2 = Temp
		Output = Input1 - Input2
		op = '-'
	elif Operation == 2:
		# multiply
		Output = Input1 * Input2
		op = '*'
	elif Operation == 3:
		# divide
		Output = int(Input1 / Input2)
		op = '/'

	# reverse the output chars since that's how we'll get them back from the emulator
	Output = "{}".format(Output)
	Output = Output[::-1]
	print "{} {} {} = {}".format(Input1, op, Input2, Output)
	NumChars = len(Output)

	# insert a random instruction if we have space
	if random.randint(0,1) and AvailableInstructions:
		RandomInstruction()
		AvailableInstructions -= 1

	# randomly set the OutputCountAddr register
	if random.randint(0,1) and OutputCountSet == 0:
		print "ML(R02, 0)"
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	# insert a random instruction if we have space
	if random.randint(0,1) and AvailableInstructions:
		RandomInstruction()
		AvailableInstructions -= 1

	# mix up the order of sending the OutputBufAddr, Input1, Input2, and NumChars to send
	rnd = random.randint(0,0)
	if rnd == 0:
		print "ML(R01, 0)"
		code.append(ML(R01, 0))
		print "MH(R01, {})".format(OutputBufAddr)
		code.append(MH(R01, OutputBufAddr))
		print "ML(R03, {})".format(NumChars)
		code.append(ML(R03, NumChars))
		print "ML(R07, {})".format(Input1)
		code.append(ML(R07, Input1))
	elif rnd == 1:
		code.append(ML(R01, 0))
		code.append(MH(R01, OutputBufAddr))
		code.append(ML(R03, NumChars))
		code.append(ML(R07, Input1))
	elif rnd == 2:
		code.append(ML(R01, 0))
		code.append(ML(R03, NumChars))
		code.append(MH(R01, OutputBufAddr))
		code.append(ML(R07, Input1))
	elif rnd == 3:
		code.append(ML(R03, NumChars))
		code.append(ML(R01, 0))
		code.append(MH(R01, OutputBufAddr))
		code.append(ML(R07, Input1))

	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# randomly set the OutputCountAddr register
	if random.randint(0,1) and OutputCountSet == 0:
		# set the OutputCountAddr register
		print "ML(R02, {})".format(0)
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	# insert a random instruction if we have space
	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# perform the selected math operation
	if Operation == 0:
		# add
		print "ADI(R08, R07, {})".format(Input2)
		code.append(ADI(R08, R07, Input2))
	elif Operation == 1:
		# subtract
		print "SBI(R08, R07, {})".format(Input2)
		code.append(SBI(R08, R07, Input2))
	elif Operation == 2:
		# multiply
		print "MUI(R08, R07, {})".format(Input2)
		code.append(MUI(R08, R07, Input2))
	elif Operation == 3:
		print "DVI(R08, R07, {})".format(Input2)
		code.append(DVI(R08, R07, Input2))

	# create the loop to write the chars to the buffer
	print "MDI(R08, R08, 10)"
	code.append(MDI(R09, R08, 10))
	print "ADI(R01, R01, {})".format(ord('0'))
	code.append(ADI(R09, R09, ord('0')))
	print "STS(R09, R01)"
	code.append(STS(R09, R01))
	print "ADI(R01, R01, 1)"
	code.append(ADI(R01, R01, 1))
	print "DVI(R08, R08, 10)"
	code.append(DVI(R08, R08, 10))
	print "B(N, -18)"
	code.append(B(N, -18))

	# insert a random instruction if we have space
	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1

	# set the OutputCountAddr register if it hasn't been set yet
	if OutputCountSet == 0:
		# set the OutputCountAddr register
		print "ML(R02, {})".format(0)
		code.append(ML(R02, 0))
		print "MH(R02, {})".format(OutputCountAddr)
		code.append(MH(R02, OutputCountAddr))
		OutputCountSet = 1

	print "ML(R03, {})".format(NumChars)
	code.append(ML(R03, NumChars))
	print "STT(R03, R02)"
	code.append(STT(R03, R02))

	# insert a random instruction if we have space
	if random.randint(0,1) and AvailableInstructions:
		code.append(RandomInstruction())
		AvailableInstructions -= 1


	print "HT()"
	code.append(HT())

	Sc = MakeSc(code)
	Solution = GenerateSolution(Sc)
	SendSc(s, Sc)
	time.sleep(0.1)

	return ([Output, Solution])

def MakeSc(code):
	sc = []
	for c in code:
		codelen = len(c)
		if codelen == 3:
			instr = int(c, 16)
			sc.append(instr & 0x1ff)
		elif codelen == 5:
			instr = int(c, 16)
			sc.append(instr & 0x1ff)
			sc.append((instr>>9) & 0x1ff)
		elif codelen == 7:
			instr = int(c, 16)
			sc.append((instr>>9) & 0x1ff)
			sc.append((instr>>18) & 0x1ff)
			sc.append(instr & 0x1ff)
		elif codelen == 14:
			instr = int(c[0:7], 16)
			sc.append((instr>>8) & 0x1ff)
			sc.append((instr>>17) & 0x1ff)
			sc.append(instr & 0x1ff)
			instr = int(c[7:14], 16)
			sc.append((instr>>8) & 0x1ff)
			sc.append((instr>>17) & 0x1ff)
			sc.append(instr & 0x1ff)
		else:
			print "Invalid instruction length {} for {}".format(codelen, c)
			sys.exit(0)
	
	# HT
#	sc.append(192)
#	sc.append(320)

	if len(sc) > 54:
		print "Code too long {}".format(len(sc))
		sys.exit(0)

	for x in range(len(sc), 54):
		sc.append(0)

	return sc

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
		c = int(Bits[i+1:i+9], 2)
		if c > 31 and c < 128:
			Output += chr(int(Bits[i+1:i+9], 2))
		else:
			Output += hex(int(Bits[i+1:i+9], 2)) + " "

	return Output

def Data_9_to_8_fixed(InData):
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

def SendSc(s, sc):
	Data = s.recv(4096)
	Data = Data_9_to_8_fixed(Data)
	sys.stdout.write(Data)
	sys.stdout.flush()

	out = ",".join(str(x) for x in sc)
	Data = Data_8_to_9(out+"\n")
	sys.stdout.write(out+"\n")
	s.send(Data)

def SolveCube(s, solution):
	print "Solving the cube"
	for action in solution.split(' '):
		# send the action
		Data = Data_8_to_9(action+"\n")
#		sys.stdout.write(action+"\n")
		s.send(Data)

		# make sure we got the expected response
		time.sleep(0.1)
		Data = s.recv(4096)
		Data8 = Data_9_to_8_fixed(Data)
		if re.search("Invalid", Data8):
			# unexpected response
			print "Unexpected response: {}".format(Data8)
			sys.exit(0)
		if re.search("Failed", Data8):
			# unexpected response
			print "Unexpected response: {}".format(Data8)
			sys.exit(0)
		m = re.search("^.*Solved.(.*)$", Data8, re.DOTALL)
		if m:
			print Data8
			return m.group(1)
#		sys.stdout.write(Data8)

# the neatlibc rand functions aren't standard
ct = CDLL('./rnd.so')
SeedRand = ct.SeedRand
SeedRand.argtypes = [c_uint]
SeedRand.restype = None
GimmeRand = ct.GimmeRand
GimmeRand.argtypes = None
GimmeRand.restype = c_uint

if len(sys.argv) == 3:
	s = socket.create_connection([sys.argv[1], sys.argv[2]])
else:
	s = socket.create_connection(['127.0.0.1', 4000])


# Pick a test to run and send it
choice = random.randint(0,1)
print "Choice: {}".format(choice)
if choice == 0:
	(ExpectedResponse, Solution) = Test_Echo(s)
elif choice == 1:
	(ExpectedResponse, Solution) = Test_Math(s)

# Read the initial output
select.select([s.fileno(), 0], [], [])
Data = s.recv(4096)
Data8 = Data_9_to_8_fixed(Data)
#sys.stdout.write(Data8)

# Solve the Cube and validate the response
#Response = SolveCube(s, Polls[choice]['solution'])
Response = SolveCube(s, Solution)
if Response == ExpectedResponse:
	print "Correct"
	sys.exit(0)
else:
	print "Incorrect: |{}| != {}".format(Response, ExpectedResponse)
	sys.exit(-1)
