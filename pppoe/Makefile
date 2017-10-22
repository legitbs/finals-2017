CC = ncc-inv
LD = nld
AS = las
output = test

CFLAGS = -O2 -I. -I../../lightning/cLEMENCy/neat/neatlibc
LDFLAGS = -f -mc 10000 -L../../lightning/cLEMENCy/neat/neatlibc

SHELL := /bin/bash
export PATH := $(PATH):../../lightning/cLEMENCy/bin

all: test

%.o: %.s
	$(AS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^

OBJS1 = $(patsubst %.c,%.o,$(wildcard *.c))
OBJS2 = $(patsubst %.s,%.o,$(wildcard *.s))

test:	$(OBJS1) $(OBJS2)
	$(LD) $(LDFLAGS) -o $(output) -lc-inv $(OBJS1) $(OBJS2) 

clean:
	rm -f *.o $(output).map $(output).dmp $(output)
