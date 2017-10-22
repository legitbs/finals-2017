#all: rubix rubix-clemency
all: rubix-clemency rnd

rnd: rnd.c
	gcc -fPIC -shared -o rnd.so rnd.c

rubix-clemency: rubix.c cube.h
	/home/joe/lbs/lightning/cLEMENCy/neat/neatcc/ncc -I/home/joe/lbs/lightning/cLEMENCy/neat/neatlibc -o rubix-clemency.o rubix.c
	/home/joe/lbs/lightning/cLEMENCy/neat/ld/nld -f -o rubix.bin -L /home/joe/lbs/lightning/cLEMENCy/neat/neatlibc -lc rubix-clemency.o

#rubix: rubix.c cube.h
#	gcc -o rubix rubix.c
