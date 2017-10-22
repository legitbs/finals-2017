#!/usr/bin/python

from cLEMENCy import *

print OR(R02, R01, R08)
print ML(R01, 0x0)
print MH(R01, 0x14040)
print STS(R08, R01, MemOffset = 1)
