#!/usr/bin/env make

CC = ncc
LD = nld
AS = las
output = bstracker

CFLAGS = -O2 -I. -I../../lightning/cLEMENCy/neat/neatlibc
LDFLAGS = -f -mc 10000 -L../../lightning/cLEMENCy/neat/neatlibc

SHELL := /bin/bash
export PATH := $(PATH):../../lightning/cLEMENCy/bin

all: bstracker

%.o: %.s
	$(AS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^

OBJS1 = $(patsubst %.c,%.o,$(wildcard *.c))
OBJS2 = $(patsubst %.s,%.o,$(wildcard *.s))

bstracker: $(OBJS1) $(OBJS2)
	$(LD) $(LDFLAGS) -o $(output) -lc $(OBJS1) $(OBJS2)

clean:
	rm -f *.o *.map *.dmp $(output)
