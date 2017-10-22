#!/usr/bin/python

import sys
import re

out = "sc = ["
line = sys.stdin.readline().rstrip()
while line:
#	print line
# main  0005df7: 4820000          ml	R01, 0x0
	m = re.search(r"\s+([0-9a-f]+)\s+", line)
	if m:
		if len(m.group(1)) == 7:
			instr = int(m.group(1), 16)
			out += str((instr>>9) & 0x1ff) + ","
			out += str((instr>>18) & 0x1ff) + ","
			out += str(instr & 0x1ff) + ","
		elif len(m.group(1)) == 14:
			instr = int(m.group(1)[0:7], 16)
			out += str((instr>>8) & 0x1ff) + ","
			out += str((instr>>17) & 0x1ff) + ","
			out += str(instr & 0x1ff) + ","
			instr = int(m.group(1)[7:14], 16)
			out += str((instr>>8) & 0x1ff) + ","
			out += str((instr>>17) & 0x1ff) + ","
			out += str(instr & 0x1ff) + ","
	line = sys.stdin.readline().rstrip()

# HT
out += str(192) + ","
out += str(320) + ","
out += str(0) + ","
out += str(0) + ","
out += str(0) + ","
out += str(0) + "]"

print out
