TARGET = picturemgr
LIBS = -lc
INCLUDES = ../../../lightning/cLEMENCy/neat/neatlibc
CC = ../../../lightning/cLEMENCy/bin/ncc
LD = ../../../lightning/cLEMENCy/bin/nld
LAS = ../../../lightning/cLEMENCy/bin/las

CFLAGS = 

DEFINES = 

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c)) $(patsubst %s, %o, $(wildcard *.s))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(DEFINES) $(CFLAGS) -I$(INCLUDES)  -o $@ $< 

%.o: %.s 
	$(LAS) -o $@ $<

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(LD) -f -o $@ -L$(INCLUDES) $(LIBS) $(OBJECTS)

clean:
	-rm -f *.o
	-rm -f $(TARGET)
