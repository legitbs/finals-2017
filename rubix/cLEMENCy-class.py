import base64

class cLEMENCy:
	# Registers
	R00 = 0
	R01 = 1
	R02 = 2
	R03 = 3
	R04 = 4
	R05 = 5
	R06 = 6
	R07 = 7
	R08 = 8
	R09 = 9
	R10 = 10
	R11 = 11
	R12 = 12
	R13 = 13
	R14 = 14
	R15 = 15
	R16 = 16
	R17 = 17
	R18 = 18
	R19 = 19
	R20 = 20
	R21 = 21
	R22 = 22
	R23 = 23
	R24 = 24
	R25 = 25
	R26 = 26
	R27 = 27
	R28 = 28
	ST  = 29
	RA  = 30
	PC  = 31

	# branch conditions
	N = 0
	E = 1
	L = 2
	LE = 3
	G = 4
	GE = 5
	NO = 6
	O = 7
	NS = 8
	S = 9
	SL = 10
	SLE = 11
	SG = 12
	SGE = 13
	ALWAYS = 15

	def Bin2Hex(self, Bin):
		Hex = ''
		if len(Bin) == 54:
			Hex = hex(int(Bin[0:2], 2))
			for x in xrange(2, 54, 4):
				Hex += hex(int(Bin[x:x+4], 2))
		elif len(Bin) == 27:
			Hex = hex(int(Bin[0:3], 2))
			for x in xrange(3, 27, 4):
				Hex += hex(int(Bin[x:x+4], 2))
		elif len(Bin) == 17:
			Hex = hex(int(Bin[0:1], 2))
			for x in xrange(1, 17, 4):
				Hex += hex(int(Bin[x:x+4], 2))
		else:
			return "Unrecognized instruction length: {}".format(len(Hex))
	
		return(Hex.replace('0x', ''))

	def Hex2Bytes(self, Hex):
		if len(Hex) % 2:
			return(base64.b16decode(Hex+"0"))
		return(base64.b16decode(Hex))

	def AD(self, rA, rB, rC, UF = 1):
		Instr = '0000000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADC(self, rA, rB, rC, UF = 1):
		Instr = '0100000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADCI(self, rA, rB, Immediate, UF = 1):
		Instr = '0100000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADCIM(self, rA, rB, Immediate, UF = 1):
		Instr = '0100010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADCM(self, rA, rB, rC, UF = 1):
		Instr = '0100010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADF(self, rA, rB, rC, UF = 1):
		Instr = '0000001'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADFM(self, rA, rB, rC, UF = 1):
		Instr = '0000011'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADI(self, rA, rB, Immediate, UF = 1):
		Instr = '0000000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADIM(self, rA, rB, Immediate, UF = 1):
		Instr = '0000010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ADM(self, rA, rB, rC, UF = 1):
		Instr = '0000010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def AN(self, rA, rB, rC, UF = 1):
		Instr = '0010100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ANI(self, rA, rB, Immedate, UF = 1):
		Instr = '0010100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immedate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ANM(self, rA, rB, rC, UF = 1):
		Instr = '0010110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def B(self, Condition, Offset, UF = 1):
		Instr = '110000'
		Instr += ("0"*8+(bin(Condition)[2:]))[-4:]
		Instr += ("0"*17+(bin(Offset)[2:]))[-17:]

		return(self.Bin2Hex(Instr))

	def BF(self, rA, rB, UF = 1):
		Instr = '101001100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '1000000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def BFM(self, rA, rB, UF = 1):
		Instr = '101001110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '1000000'
		Instr += ("0"*26+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def BR(self, Condition, rA):
		Instr = '110010'
		Instr += ("0"*8+(bin(Condition)[2:]))[-4:]
		Instr += ("0"*17+(bin(rA)[2:]))[-5:]
		Instr += '0000'

		return(self.Bin2Hex(Instr))

	def BRA(self, Location):
		Instr = '111000100'
		Instr += ("0"*27+(bin(Location)[2:]))[-27:]

		return(self.Bin2Hex(Instr))

	def BRR(self, Offset):
		Instr = '111000000'
		Instr += ("0"*27+(bin(Offset)[2:]))[-27:]

		return(self.Bin2Hex(Instr))

	def C(self, Condition, Offset):
		Instr = '110101'
		Instr += ("0"*8+(bin(Condition)[2:]))[-4:]
		Instr += ("0"*17+(bin(Offset)[2:]))[-17:]

		return(self.Bin2Hex(Instr))

	def CAA(self, Location):
		Instr = '111001100'
		Instr += ("0"*27+(bin(Location)[2:]))[-27:]

		return(self.Bin2Hex(Instr))

	def CAR(self, Offset):
		Instr = '111001000'
		Instr += ("0"*27+(bin(Offset)[2:]))[-27:]

		return(self.Bin2Hex(Instr))

	def CM(self, rA, rB):
		Instr = '10111000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]

		return(self.Bin2Hex(Instr))

	def CMF(self, rA, rB):
		Instr = '10111010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]

		return(self.Bin2Hex(Instr))

	def CMFM(self, rA, rB):
		Instr = '10111110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]

		return(self.Bin2Hex(Instr))

	def CMI(self, rA, Immediate):
		Instr = '10111001'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*14+(bin(Immediate)[2:]))[-14:]

		return(self.Bin2Hex(Instr))

	def CMIM(self, rA, Immediate):
		Instr = '10111101'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*14+(bin(Immediate)[2:]))[-14:]

		return(self.Bin2Hex(Instr))

	def CMM(self, rA, Immediate):
		Instr = '10111100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]

		return(self.Bin2Hex(Instr))

	def CR(self, Condition, rA):
		Instr = '110111'
		Instr += ("0"*8+(bin(Condition)[2:]))[-4:]
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += '000'

		return(self.Bin2Hex(Instr))

	def DBRK(self):
		Instr = '11111111111111111'

		return(self.Bin2Hex(Instr))

	def DI(self, rA):
		Instr = '101000000101'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += '0'

		return(self.Bin2Hex(Instr))

	def DMT(self, rA, rB, rC):
		Instr = '0110100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '00000'

		return(self.Bin2Hex(Instr))

	def DV(self, rA, rB, rC, UF = 1):
		Instr = '0001100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVF(self, rA, rB, rC, UF = 1):
		Instr = '0001101'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVFM(self, rA, rB, rC, UF = 1):
		Instr = '0001111'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVI(self, rA, rB, Immediate, UF = 1):
		Instr = '0001100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVIM(self, rA, rB, Immediate, UF = 1):
		Instr = '0001110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVIS(self, rA, rB, Immediate, UF = 1):
		Instr = '0001100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '11'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVISM(self, rA, rB, Immediate, UF = 1):
		Instr = '0001110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '11'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVM(self, rA, rB, rC, UF = 1):
		Instr = '0001110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVS(self, rA, rB, rC, UF = 1):
		Instr = '0001100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0010'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def DVSM(self, rA, rB, rC, UF = 1):
		Instr = '0001110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0010'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def EI(self, rA):
		Instr = '101000000100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += '0'

		return(self.Bin2Hex(Instr))

	def FTI(self, rA, rB):
		Instr = '101000101'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '00000000'

		return(self.Bin2Hex(Instr))

	def FTIM(self, rA, rB):
		Instr = '101000111'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '00000000'

		return(self.Bin2Hex(Instr))

	def HT(self):
		Instr = '101000000011000000'

		return(self.Bin2Hex(Instr))

	def IR(self):
		Instr = '101000000001000000'

		return(self.Bin2Hex(Instr))

	def ITF(self, rA, rB):
		Instr = '101000100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '00000000'

		return(self.Bin2Hex(Instr))

	def ITFM(self, rA, rB):
		Instr = '101000110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += '00000000'

		return(self.Bin2Hex(Instr))

	def LDS(self, rA, rB, RegisterCount = 0, AdjustRb = 0, MemoryOffset = 0):
		Instr = '1010100'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(RegCount)[2:]))[-5:]
		Instr += ("0"*8+(bin(AdjustRb)[2:]))[-2:]
		Instr += ("0"*26+(bin(MemOffset)[2:]))[-27:]
		Instr += '000'

		return(self.Bin2Hex(Instr))

	def LDT(self, rA, rB, RegisterCount = 0, AdjustRb = 0, MemoryOffset = 0):
		Instr = '1010110'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(RegCount)[2:]))[-5:]
		Instr += ("0"*8+(bin(AdjustRb)[2:]))[-2:]
		Instr += ("0"*26+(bin(MemOffset)[2:]))[-27:]
		Instr += '000'

		return(self.Bin2Hex(Instr))

	def LDW(self, rA, rB, RegisterCount = 0, AdjustRb = 0, MemoryOffset = 0):
		Instr = '1010101'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(RegCount)[2:]))[-5:]
		Instr += ("0"*8+(bin(AdjustRb)[2:]))[-2:]
		Instr += ("0"*26+(bin(MemOffset)[2:]))[-27:]
		Instr += '000'

		return(self.Bin2Hex(Instr))

	def MD(self, rA, rB, rC, UF = 1):
		Instr = '0010000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDF(self, rA, rB, rC, UF = 1):
		Instr = '0010001'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDFM(self, rA, rB, rC, UF = 1):
		Instr = '0010011'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDI(self, rA, rB, Immediate, UF = 1):
		Instr = '0010000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDIM(self, rA, rB, Immediate, UF = 1):
		Instr = '0010010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDIS(self, rA, rB, Immediate, UF = 1):
		Instr = '0010000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '11'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDISM(self, rA, rB, Immediate, UF = 1):
		Instr = '0010010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '11'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDM(self, rA, rB, rC, UF = 1):
		Instr = '0010010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDS(self, rA, rB, rC, UF = 1):
		Instr = '0010000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0010'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def MDSM(self, rA, rB, rC, UF = 1):
		Instr = '0010010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0010'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def ML(self, rA, Immediate):
		Instr = '10010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*26+(bin(Immediate)[2:]))[-17:]
	
		return(self.Bin2Hex(Instr))
	
	def MH(self, rA, Immediate):
		Instr = '10001'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*26+(bin(Immediate)[2:]))[-17:]
	
		return(self.Bin2Hex(Instr))

	def MS(self, rA, Immediate):
		Instr = '10011'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*26+(bin(Immediate)[2:]))[-17:]
	
		return(self.Bin2Hex(Instr))
	
	def MU(self, rA, rB, rC, UF = 1):
		Instr = '0001000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]
	
		return(self.Bin2Hex(Instr))

	def MUF(self, rA, rB, rC, UF = 1):
		Instr = '0001001'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]
	
		return(self.Bin2Hex(Instr))

	def MUFM(self, rA, rB, rC, UF = 1):
		Instr = '0001011'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]
	
		return(self.Bin2Hex(Instr))

	def MUI(self, rA, rB, Immediate, UF = 1):
		Instr = '0001000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]
	
		return(self.Bin2Hex(Instr))

	def MIUM(self, rA, rB, Immediate, UF = 1):
		Instr = '0001010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '01'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]
	
		return(self.Bin2Hex(Instr))

	def MUIS(self, rA, rB, Immediate, UF = 1):
		Instr = '0001000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(Immediate)[2:]))[-7:]
		Instr += '11'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def OR(self, rA, rB, rC, UF = 1):
		Instr = '0011000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(rC)[2:]))[-5:]
		Instr += '0000'
		Instr += ("0"*8+(bin(UF)[2:]))[-1:]

		return(self.Bin2Hex(Instr))

	def STS(self, rA, rB, RegCount = 0, AdjustRb = 0, MemOffset = 0):
		Instr = '1011000'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(RegCount)[2:]))[-5:]
		Instr += ("0"*8+(bin(AdjustRb)[2:]))[-2:]
		Instr += ("0"*26+(bin(MemOffset)[2:]))[-27:]
		Instr += '000'
	
		return(self.Bin2Hex(Instr))

	def STT(self, rA, rB, RegCount = 0, AdjustRb = 0, MemOffset = 0):
		Instr = '1011010'
		Instr += ("0"*8+(bin(rA)[2:]))[-5:]
		Instr += ("0"*8+(bin(rB)[2:]))[-5:]
		Instr += ("0"*8+(bin(RegCount)[2:]))[-5:]
		Instr += ("0"*8+(bin(AdjustRb)[2:]))[-2:]
		Instr += ("0"*26+(bin(MemOffset)[2:]))[-27:]
		Instr += '000'
	
		return(self.Bin2Hex(Instr))

