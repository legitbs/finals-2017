all: babyecho

babyecho: babyecho.c
	/home/joe/lbs/lightning/cLEMENCy/neat/neatcc/ncc -I/home/joe/lbs/lightning/cLEMENCy/neat/neatlibc -o babyecho.o babyecho.c
	/home/joe/lbs/lightning/cLEMENCy/neat/ld/nld -f -o babyecho.bin -L /home/joe/lbs/lightning/cLEMENCy/neat/neatlibc -lc babyecho.o
