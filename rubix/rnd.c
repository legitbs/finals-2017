#include <stdio.h>
#include <stdlib.h>

static unsigned int r;

void SeedRand(unsigned int s) {
	r = s;
}

unsigned int GimmeRand(void) {
	r = r * 103515245 + 12345;
	return(r & 0x7ffffff);
}
