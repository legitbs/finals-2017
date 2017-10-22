#include <stdio.h>
#include <stdlib.h>
//#include <signal.h>
#include <string.h>

#define TIMEOUT 20

int read_until(char *buf, int len, char delim[10]) {
	int total_len = 0;
	char c[10];
	long retval;
	int i;

	while (total_len < len) {
//		if (read(0, &c, 1) <= 0) {
		if ((retval = read(c, 1)) < 0) {
			exit(-1);
		}
		if (retval == 0) {
//			sleep(1);
			for (i = 0; i < 4000; ) {
				i++;
			}
			continue;
		}
		if (c[0] == delim[0]) {
			buf[total_len] = '\0';
			return(total_len);
		}
		buf[total_len++] = c[0];
	}
	buf[total_len-1] = '\0';

	return(total_len-1);
}

void tooslow(int sig) {
	printf("too slow\n");
	exit(-1);
}

void filter(char *fmtstr) {
	int i;

	if (strlen(fmtstr) < 2) {
		return;
	}

	// intentionally weak checks...just meant as a distraction
	for (i = 0; i < strlen(fmtstr)-1; i++) {
		if (fmtstr[i] == '%' && fmtstr[i+1] == 'n') {
			fmtstr[i] = '_';
		}
	}

}

void Loop(char buf[1024], int *done, int *len) {

	while (!*done) {
		*len = *len < 1024 ? *len : 1023;
		printf("Reading %d bytes\n", *len);
		read_until(buf, *len, "\n");
//		filter(buf);
		printf(buf);
		printf("\n");
//		alarm(TIMEOUT);
	}
}

int main(void) {
	char buf[1024];
	char *c = buf;
	int done = 0;
	int len = 200;

//	setvbuf(stdout, NULL, _IONBF, 0);

//	signal(SIGALRM, tooslow);
//	alarm(TIMEOUT);

//	while (!done) {
//		done = 0xffff;
//		len = len < 1024 ? len : 1023;
//		printf("Reading %d bytes\n", len);
//		read_until(buf, len, "\n");
////		filter(buf);
//		printf(buf);
//		printf("\n");
////		alarm(TIMEOUT);
//	}
	Loop(buf, &done, &len);

	return(0);
}
