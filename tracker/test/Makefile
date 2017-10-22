#!/usr/bin/env make

X86_CC = gcc
X86_CFLAGS = -m32 -g -O2 -I..

CCY_CC = ncc
CCY_LD = nld
CCY_AS = las
CCY_CFLAGS = -O2 -I.. -I../../../lightning/cLEMENCy/neat/neatlibc
CCY_LDFLAGS = -f -mc 10000 -L../../../lightning/cLEMENCy/neat/neatlibc

SHELL := /bin/bash
export PATH := $(PATH):../../../lightning/cLEMENCy/bin

all: list_test_x86 list_test_ccy vector_test_x86 vector_test_ccy fixed_test_x86 fixed_test_ccy instrument_test_x86 instrument_test_ccy pattern_test_x86 pattern_test_ccy tracker_test_x86 tracker_test_ccy mix_test_x86

assert_ccy:
	$(CCY_CC) -c $(CCY_CFLAGS) assert.c

list_test_x86: 
	$(X86_CC) $(X86_CFLAGS) list_test.c ../list.c -o list_test_x86

list_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../list.c
	$(CCY_CC) -c $(CCY_CFLAGS) list_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o list_test_ccy -lc list_test.o ../list.o assert.o

vector_test_x86: 
	$(X86_CC) $(X86_CFLAGS) vector_test.c ../vector.c -o vector_test_x86

vector_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../vector.c
	$(CCY_CC) -c $(CCY_CFLAGS) vector_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o vector_test_ccy -lc vector_test.o ../vector.o assert.o

fixed_test_x86: 
	$(X86_CC) $(X86_CFLAGS) fixed_test.c ../fixed.c -o fixed_test_x86

fixed_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../fixed.c
	$(CCY_CC) -c $(CCY_CFLAGS) fixed_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o fixed_test_ccy -lc fixed_test.o ../fixed.o assert.o

instrument_test_x86:
	$(X86_CC) $(X86_CFLAGS) instrument_test.c ../instrument.c ../sample.c -o instrument_test_x86

instrument_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../instrument.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../sample.c
	$(CCY_CC) -c $(CCY_CFLAGS) instrument_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o instrument_test_ccy -lc instrument_test.o ../instrument.o ../sample.o assert.o

pattern_test_x86: 
	$(X86_CC) $(X86_CFLAGS) pattern_test.c ../pattern.c ../event.c ../vector.c -o pattern_test_x86

pattern_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../pattern.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../event.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../vector.c
	$(CCY_CC) -c $(CCY_CFLAGS) pattern_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o pattern_test_ccy -lc pattern_test.o ../pattern.o ../event.o ../vector.o assert.o

tracker_test_x86:
	$(X86_CC) $(X86_CFLAGS) tracker_test.c ../tracker.c ../channel.c ../pattern.c ../event.c ../instrument.c ../sample.c ../vector.c ../list.c ../fixed.c -o tracker_test_x86

tracker_test_ccy: assert_ccy
	$(CCY_CC) -c $(CCY_CFLAGS) ../tracker.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../channel.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../pattern.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../event.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../instrument.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../sample.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../vector.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../list.c
	$(CCY_CC) -c $(CCY_CFLAGS) ../fixed.c
	$(CCY_CC) -c $(CCY_CFLAGS) tracker_test.c
	$(CCY_LD) $(CCY_LDFLAGS) -o tracker_test_ccy -lc tracker_test.o ../tracker.o ../channel.o ../pattern.o ../event.o ../instrument.o ../sample.o ../vector.o ../list.o ../fixed.o assert.o

mix_test_x86: 
	$(X86_CC) $(X86_CFLAGS) mix_test.c -o mix_test -lm

clean:
	rm -f *.o  ../*.o *.map ../*.map *.dmp ../*.dmp
	rm -f list_test_ccy vector_test_ccy fixed_test_ccy instrument_test_ccy pattern_test_ccy tracker_test_ccy mix_test_ccy
	rm -f list_test_x86 vector_test_x86 fixed_test_x86 instrument_test_x86 pattern_test_x86 tracker_test_x86 mix_test_x86
