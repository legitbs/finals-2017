all: forth_v2

NEATDIR = /home/ubuntu/cLEMENCy/neat

forth_v2: main.c
	$(NEATDIR)/neatcc/ncc -DV0 -I$(NEATDIR)/neatlibc -o main.o main.c
	$(NEATDIR)/neatcc/ncc -DV0 -I$(NEATDIR)/neatlibc -o strtok.o strtok.c
	$(NEATDIR)/neatcc/ncc -DV0 -I$(NEATDIR)/neatlibc -o strspn.o strspn.c
	$(NEATDIR)/neatcc/ncc -DV0 -I$(NEATDIR)/neatlibc -o strpbrk.o strpbrk.c
	$(NEATDIR)/ld/nld -f -o main.bin -L $(NEATDIR)/neatlibc -lc main.o strtok.o strspn.o strpbrk.o

clean:
	rm *.o
	rm main.bin
	rm main.bin.map

