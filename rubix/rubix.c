#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cube.h"

/* Cube Layout
              0  1  2
              3  4  5
              6  7  8

 9 10 11      18 19 20      27 28 29      45 46 47
12 13 14      21 22 23      30 31 32      48 49 50
15 16 17      24 25 26      33 34 35      51 52 53

              36 37 38
              39 40 41
              42 43 44
*/

#define CUBE_SIZE (54)
char *Mem;
char *Cube;
char *OriginalCube;
char *TextCube;
#define MAX_ITERATIONS (30)

void PrintCube(char C[CUBE_SIZE]) {

	// Print the top face
	printf("                Top\n");
	printf("             %02x %02x %02x\n", T0, T1, T2);
	printf("             %02x %02x %02x\n", T3, T4, T5);
	printf("             %02x %02x %02x\n", T6, T7, T8);
	printf("\n");

	// Print the left, front, right face
	printf("  Left         Front       Right\n");
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x\n", L0, L1, L2, F0, F1, F2, R0, R1, R2);
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x\n", L3, L4, L5, F3, F4, F5, R3, R4, R5);
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x\n", L6, L7, L8, F6, F7, F8, R6, R7, R8);
	printf("\n");

	// Print the bottom face
	printf("              Bottom\n");
	printf("             %02x %02x %02x\n", B0, B1, B2);
	printf("             %02x %02x %02x\n", B3, B4, B5);
	printf("             %02x %02x %02x\n", B6, B7, B8);
	printf("\n");

	// Print the back face
	printf("               Back\n");
	printf("             %02x %02x %02x\n", A0, A1, A2);
	printf("             %02x %02x %02x\n", A3, A4, A5);
	printf("             %02x %02x %02x\n", A6, A7, A8);

}

void PrintCube2(char C[CUBE_SIZE]) {

	// Print the top face
	printf("                Top\n");
	printf("               %02x %02x %02x\n", T0, T1, T2);
	printf("              %02x  %02x  %02x\n", T3, T4, T5);
	printf("             %02x   %02x   %02x\n", T6, T7, T8);
	printf("\n");

	// Print the left, front, right face
	printf("  Left         Front       Right\n");
	printf("        %02x   %02x   %02x   %02x   %02x          \n", L2, F0, F1, F2, R0);
	printf("    %02x                          %02x     \n", L1, R1);
	printf("%02x                                  %02x\n", L0, R2);
	printf("%02x  %02x  %02x   %02x   %02x   %02x   %02x  %02x  %02x\n", L3, L4, L5, F3, F4, F5, R3, R4, R5);
	printf("%02x                                  %02x\n", L6, R8);
	printf("    %02x                          %02x     \n", L7, R7);
	printf("        %02x   %02x   %02x   %02x   %02x          \n", L8, F6, F7, F8, R6);
	printf("\n");

	// Print the bottom face
	printf("              Bottom\n");
	printf("             %02x   %02x   %02x\n", B0, B1, B2);
	printf("              %02x  %02x  %02x\n", B3, B4, B5);
	printf("               %02x %02x %02x\n", B6, B7, B8);
	printf("\n");

	// Print the back face
	printf("               Back\n");
	printf("             %02x %02x %02x\n", A0, A1, A2);
	printf("             %02x %02x %02x\n", A3, A4, A5);
	printf("             %02x %02x %02x\n", A6, A7, A8);

}

void PrintCube3(char C[CUBE_SIZE]) {

	// Print the top face
	printf("                Top\n");
	printf("             %02x %02x %02x\n", T0, T1, T2);
	printf("             %02x %02x %02x\n", T3, T4, T5);
	printf("             %02x %02x %02x\n", T6, T7, T8);
	printf("\n");

	// Print the left, front, right face
	printf("  Left         Front       Right       Back\n");
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x    %02x %02x %02x\n", L0, L1, L2, F0, F1, F2, R0, R1, R2, A0, A1, A2);
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x    %02x %02x %02x\n", L3, L4, L5, F3, F4, F5, R3, R4, R5, A3, A4, A5);
	printf("%02x %02x %02x     %02x %02x %02x    %02x %02x %02x    %02x %02x %02x\n", L6, L7, L8, F6, F7, F8, R6, R7, R8, A6, A7, A8);
	printf("\n");

	// Print the bottom face
	printf("              Bottom\n");
	printf("             %02x %02x %02x\n", B0, B1, B2);
	printf("             %02x %02x %02x\n", B3, B4, B5);
	printf("             %02x %02x %02x\n", B6, B7, B8);
	printf("\n");

}

void PrintCube4(char C[CUBE_SIZE]) {

	// Print the top face
	printf("                Top\n");
	fflush(stdout);
	printf("             %s %s %s\n", Cube2Text[T0], Cube2Text[T1], Cube2Text[T2]);
	fflush(stdout);
	printf("             %s %s %s\n", Cube2Text[T3], Cube2Text[T4], Cube2Text[T5]);
	fflush(stdout);
	printf("             %s %s %s\n", Cube2Text[T6], Cube2Text[T7], Cube2Text[T8]);
	fflush(stdout);
	printf("\n");
	fflush(stdout);

	// Print the left, front, right face
	printf("  Left         Front       Right       Back\n");
	printf("%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", Cube2Text[L0], Cube2Text[L1], Cube2Text[L2], Cube2Text[F0], Cube2Text[F1], Cube2Text[F2], Cube2Text[R0], Cube2Text[R1], Cube2Text[R2], Cube2Text[A0], Cube2Text[A1], Cube2Text[A2]);
	printf("%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", Cube2Text[L3], Cube2Text[L4], Cube2Text[L5], Cube2Text[F3], Cube2Text[F4], Cube2Text[F5], Cube2Text[R3], Cube2Text[R4], Cube2Text[R5], Cube2Text[A3], Cube2Text[A4], Cube2Text[A5]);
	printf("%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", Cube2Text[L6], Cube2Text[L7], Cube2Text[L8], Cube2Text[F6], Cube2Text[F7], Cube2Text[F8], Cube2Text[R6], Cube2Text[R7], Cube2Text[R8], Cube2Text[A6], Cube2Text[A7], Cube2Text[A8]);
	printf("\n");

	// Print the bottom face
	printf("              Bottom\n");
	printf("             %s %s %s\n", Cube2Text[B0], Cube2Text[B1], Cube2Text[B2]);
	printf("             %s %s %s\n", Cube2Text[B3], Cube2Text[B4], Cube2Text[B5]);
	printf("             %s %s %s\n", Cube2Text[B6], Cube2Text[B7], Cube2Text[B8]);
	printf("\n");

}

void PrintCube5(char C[CUBE_SIZE]) {
	char buf[1024];

	// Print the top face
	sprintf(buf,"                Top\n");
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[T0], Cube2Text[T1], Cube2Text[T2]);
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[T3], Cube2Text[T4], Cube2Text[T5]);
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[T6], Cube2Text[T7], Cube2Text[T8]);
	sprintf(buf,"%s\n", buf);

	// Print the left, front, right face
	sprintf(buf,"%s  Left         Front       Right       Back\n", buf);
	sprintf(buf,"%s%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", buf, Cube2Text[L0], Cube2Text[L1], Cube2Text[L2], Cube2Text[F0], Cube2Text[F1], Cube2Text[F2], Cube2Text[R0], Cube2Text[R1], Cube2Text[R2], Cube2Text[A0], Cube2Text[A1], Cube2Text[A2]);
	sprintf(buf,"%s%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", buf, Cube2Text[L3], Cube2Text[L4], Cube2Text[L5], Cube2Text[F3], Cube2Text[F4], Cube2Text[F5], Cube2Text[R3], Cube2Text[R4], Cube2Text[R5], Cube2Text[A3], Cube2Text[A4], Cube2Text[A5]);
	sprintf(buf,"%s%s %s %s     %s %s %s    %s %s %s    %s %s %s\n", buf, Cube2Text[L6], Cube2Text[L7], Cube2Text[L8], Cube2Text[F6], Cube2Text[F7], Cube2Text[F8], Cube2Text[R6], Cube2Text[R7], Cube2Text[R8], Cube2Text[A6], Cube2Text[A7], Cube2Text[A8]);
	sprintf(buf,"%s\n", buf);

	// Print the bottom face
	sprintf(buf,"%s              Bottom\n", buf);
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[B0], Cube2Text[B1], Cube2Text[B2]);
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[B3], Cube2Text[B4], Cube2Text[B5]);
	sprintf(buf,"%s             %s %s %s\n", buf, Cube2Text[B6], Cube2Text[B7], Cube2Text[B8]);
	sprintf(buf,"%s\n", buf);

	printf("%s", buf);
}

void InitCube(unsigned char C[CUBE_SIZE], unsigned char Input[CUBE_SIZE]) {
	int i;

	for (i = 0; i < CUBE_SIZE; i++) {
		C[i] = Input[i];
	}

	for (i = 0; i < CUBE_SIZE; i++) {
		TextCube[i] = i;
	}
}

void BottomRowLeft(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F6;
	Temp2 = F7;
	Temp3 = F8;
	F6 = R6;
	F7 = R7;
	F8 = R8;
	R6 = A6;
	R7 = A7;
	R8 = A8;
	A6 = L6;
	A7 = L7;
	A8 = L8;
	L6 = Temp1;
	L7 = Temp2;
	L8 = Temp3;

	Temp1 = B0;
	Temp2 = B1;
	Temp3 = B2;
	B2 = B0;
	B1 = B3;
	B0 = B6;
	B3 = B7;
	B6 = B8;
	B7 = B5;
	B8 = Temp3;
	B5 = Temp2;
	B2 = Temp1;
}

void BottomRowRight(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F6;
	Temp2 = F7;
	Temp3 = F8;
	F6 = L6;
	F7 = L7;
	F8 = L8;
	L6 = A6;
	L7 = A7;
	L8 = A8;
	A6 = R6;
	A7 = R7;
	A8 = R8;
	R6 = Temp1;
	R7 = Temp2;
	R8 = Temp3;

	Temp1 = B0;
	Temp2 = B1;
	Temp3 = B2;
	B0 = B2;
	B1 = B5;
	B2 = B8;
	B5 = B7;
	B8 = B6;
	B7 = B3;
	B6 = Temp1;
	B3 = Temp2;
	B0 = Temp3;
}

void TopRowLeft(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F0;
	Temp2 = F1;
	Temp3 = F2;
	F0 = R0;
	F1 = R1;
	F2 = R2;
	R0 = A0;
	R1 = A1;
	R2 = A2;
	A0 = L0;
	A1 = L1;
	A2 = L2;
	L0 = Temp1;
	L1 = Temp2;
	L2 = Temp3;

	Temp1 = T0;
	Temp2 = T1;
	Temp3 = T2;
	T2 = T0;
	T1 = T3;
	T0 = T6;
	T3 = T7;
	T6 = T8;
	T7 = T5;
	T8 = Temp3;
	T5 = Temp2;
	T2 = Temp1;
}

void TopRowRight(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F0;
	Temp2 = F1;
	Temp3 = F2;
	F0 = L0;
	F1 = L1;
	F2 = L2;
	L0 = A0;
	L1 = A1;
	L2 = A2;
	A0 = R0;
	A1 = R1;
	A2 = R2;
	R0 = Temp1;
	R1 = Temp2;
	R2 = Temp3;

	Temp1 = T0;
	Temp2 = T1;
	Temp3 = T2;
	T0 = T2;
	T1 = T5;
	T2 = T8;
	T5 = T7;
	T8 = T6;
	T7 = T3;
	T6 = Temp1;
	T3 = Temp2;
	T0 = Temp3;
}

void LeftColumnDown(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = T0;
	Temp2 = T3;
	Temp3 = T6;
	T0 = A6;
	T3 = A3;
	T6 = A0;
	A6 = B0;
	A3 = B3;
	A0 = B6;
	B0 = F0;
	B3 = F3;
	B6 = F6;
	F0 = Temp1;
	F3 = Temp2;
	F6 = Temp3;

	Temp1 = L0;
	Temp2 = L1;
	Temp3 = L2;
	L2 = L0;
	L1 = L3;
	L0 = L6;
	L3 = L7;
	L6 = L8;
	L7 = L5;
	L8 = Temp3;
	L5 = Temp2;
	L2 = Temp1;
}

void LeftColumnUp(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = T0;
	Temp2 = T3;
	Temp3 = T6;
	T0 = F0;
	T3 = F3;
	T6 = F6;
	F0 = B0;
	F3 = B3;
	F6 = B6;
	B0 = A6;
	B3 = A3;
	B6 = A0;
	A6 = Temp1;
	A3 = Temp2;
	A0 = Temp3;

	Temp1 = L0;
	Temp2 = L1;
	Temp3 = L2;
	L0 = L2;
	L1 = L5;
	L2 = L8;
	L5 = L7;
	L8 = L6;
	L7 = L3;
	L6 = Temp1;
	L3 = Temp2;
	L0 = Temp3;
}

void RightColumnDown(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = T2;
	Temp2 = T5;
	Temp3 = T8;
	T2 = A8;
	T5 = A5;
	T8 = A2;
	A8 = B2;
	A5 = B5;
	A2 = B8;
	B2 = F2;
	B5 = F5;
	B8 = F8;
	F2 = Temp1;
	F5 = Temp2;
	F8 = Temp3;

	Temp1 = R0;
	Temp2 = R1;
	Temp3 = R2;
	R0 = R2;
	R1 = R5;
	R2 = R8;
	R5 = R7;
	R8 = R6;
	R7 = R3;
	R6 = Temp1;
	R3 = Temp2;
	R0 = Temp3;
}

void RightColumnUp(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = T2;
	Temp2 = T5;
	Temp3 = T8;
	T2 = F2;
	T5 = F5;
	T8 = F8;
	F2 = B2;
	F5 = B5;
	F8 = B8;
	B2 = A8;
	B5 = A5;
	B8 = A2;
	A8 = Temp1;
	A5 = Temp2;
	A2 = Temp3;

	Temp1 = R0;
	Temp2 = R1;
	Temp3 = R2;
	R2 = R0;
	R1 = R3;
	R0 = R6;
	R3 = R7;
	R6 = R8;
	R7 = R5;
	R8 = Temp3;
	R5 = Temp2;
	R2 = Temp1;
}

void MiddleRowLeft(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F3;
	Temp2 = F4;
	Temp3 = F5;
	F3 = R3;
	F4 = R4;
	F5 = R5;
	R3 = A3;
	R4 = A4;
	R5 = A5;
	A3 = L3;
	A4 = L4;
	A5 = L5;
	L3 = Temp1;
	L4 = Temp2;
	L5 = Temp3;
}

void MiddleRowRight(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F3;
	Temp2 = F4;
	Temp3 = F5;
	F3 = L3;
	F4 = L4;
	F5 = L5;
	L3 = A3;
	L4 = A4;
	L5 = A5;
	A3 = R3;
	A4 = R4;
	A5 = R5;
	R3 = Temp1;
	R4 = Temp2;
	R5 = Temp3;
}

void FrontRight(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F0;
	Temp2 = F1;
	Temp3 = F2;
	F2 = F0;
	F1 = F3;
	F0 = F6;
	F6 = F8;
	F3 = F7;
	F8 = Temp3;
	F7 = F5;
	F5 = Temp2;

	Temp1 = T6;
	Temp2 = T7;
	Temp3 = T8;
	T6 = L8;
	T7 = L5;
	T8 = L2;
	L8 = B2;
	L5 = B1;
	L2 = B0;	
	B2 = R0;
	B1 = R3;
	B0 = R6;
	R0 = Temp1;
	R3 = Temp2;
	R6 = Temp3;

}

void FrontLeft(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = F0;
	Temp2 = F1;
	Temp3 = F2;
	F0 = F2;
	F1 = F5;
	F2 = F8;
	F5 = F7;
	F8 = F6;
	F7 = F3;
	F6 = Temp1;
	F3 = Temp2;

	Temp1 = T6;
	Temp2 = T7;
	Temp3 = T8;
	T6 = R0;
	T7 = R3;
	T8 = R6;
	R6 = B0;
	R3 = B1;
	R0 = B2;	
	B0 = L2;
	B1 = L5;
	B2 = L8;
	L2 = Temp3;
	L5 = Temp2;
	L8 = Temp1;

}

void BackRight(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = A0;
	Temp2 = A1;
	Temp3 = A2;
	A0 = A2;
	A1 = A5;
	A2 = A8;
	A5 = A7;
	A8 = A6;
	A7 = A3;
	A6 = Temp1;
	A3 = Temp2;

	Temp1 = T0;
	Temp2 = T1;
	Temp3 = T2;
	T2 = L0;
	T1 = L3;
	T0 = L6;
	L0 = B6;
	L3 = B7;
	L6 = B8;
	B6 = R8;
	B7 = R5;
	B8 = R2;
	R8 = Temp3;
	R5 = Temp2;
	R2 = Temp1;

}

void BackLeft(char C[CUBE_SIZE]) {
	char Temp1, Temp2, Temp3;

	Temp1 = A0;
	Temp2 = A1;
	Temp3 = A2;
	A2 = A0;
	A1 = A3;
	A0 = A6;
	A6 = A8;
	A3 = A7;
	A8 = Temp3;
	A7 = A5;
	A5 = Temp2;

	Temp1 = T0;
	Temp2 = T1;
	Temp3 = T2;
	T0 = R2;
	T1 = R5;
	T2 = R8;
	R2 = B8;
	R5 = B7;
	R8 = B6;
	B8 = L6;
	B7 = L3;
	B6 = L0;
	L6 = Temp1;
	L3 = Temp2;
	L0 = Temp3;

}

int ReadUntil(char *Buf, int MaxLen, char Delim[10]) {
	int len;
	int TotalLen = 0;
	int i;
	struct timespec req;
	struct timespec rem;
	req.tv_sec = 0;
	req.tv_msec = 100;

	if (!Buf) {
		return(0);
	}

	memset(Buf, 0, MaxLen);

	while (TotalLen < MaxLen) {
		len = read(Buf+TotalLen, 1);
		if (len == -1) {
			return(0);
		}
		if (len == 0) {
//			for (i = 0; i < 4000; i++) {
//			}
			millisleep(&req, &rem);
			continue;
		}
		if (Buf[TotalLen] == Delim[0]) {
			Buf[TotalLen] = '\0';
			return(TotalLen);
		}
		TotalLen += 1;
		if (TotalLen >= MaxLen) {
			Buf[MaxLen-1] = '\0';
			return(MaxLen-1);
		}
	}
	
	return(TotalLen);
}

void HandleAction(char *Action) {

	if (!Action) {
		return;
	}

	if (!strcmp(Action, "U")) {
		TopRowLeft(Cube);
		TopRowLeft(TextCube);
	} else if (!strcmp(Action, "U'")) {
		TopRowRight(Cube);
		TopRowRight(TextCube);
	} else if (!strcmp(Action, "D")) {
		BottomRowLeft(Cube);
		BottomRowLeft(TextCube);
	} else if (!strcmp(Action, "D'")) {
		BottomRowRight(Cube);
		BottomRowRight(TextCube);
	} else if (!strcmp(Action, "L")) {
		LeftColumnDown(Cube);
		LeftColumnDown(TextCube);
	} else if (!strcmp(Action, "L'")) {
		LeftColumnUp(Cube);
		LeftColumnUp(TextCube);
	} else if (!strcmp(Action, "R")) {
		RightColumnUp(Cube);
		RightColumnUp(TextCube);
	} else if (!strcmp(Action, "R'")) {
		RightColumnDown(Cube);
		RightColumnDown(TextCube);
	} else if (!strcmp(Action, "F")) {
		FrontRight(Cube);
		FrontRight(TextCube);
	} else if (!strcmp(Action, "F'")) {
		FrontLeft(Cube);
		FrontLeft(TextCube);
	} else if (!strcmp(Action, "B")) {
		BackRight(Cube);
		BackRight(TextCube);
	} else if (!strcmp(Action, "B'")) {
		BackLeft(Cube);
		BackLeft(TextCube);
	} else {
		printf("Invalid Input\n");
	}
}

void ReadBytes(unsigned char Input[CUBE_SIZE], unsigned int Count) {
	char Buf[256];
	char *Token;
	char Index;
	char NumTokens;
	char BufLen;

	printf("Give me 54 Bytes(ie: 53,12,5,etc): ");
	fflush(stdout);
	if (!ReadUntil(Buf, 255, "\n")) {
		printf("Invalid input\n");
		exit(0);
	}

	Index = 0;
	Token = Buf;
	NumTokens = 0;
	BufLen = strlen(Buf);
	while (Index < BufLen) {
		if (Buf[Index] == ',') {
			Buf[Index] = '\0';
			Input[NumTokens++] = atoi(Token);
			if (NumTokens == CUBE_SIZE) {
				break;
			}
			Index++;
			Token = Buf+Index;
		} else {
			Index++;
		}
	}
	if (NumTokens < CUBE_SIZE) {
		Input[NumTokens++] = atoi(Token);
	}
		
	if (NumTokens != CUBE_SIZE) {
		printf("Invalid input\n");
		exit(0);
	}

}

/*
void ReadBytes(unsigned char Input[CUBE_SIZE], unsigned int Count) {
	unsigned int i;
	char Buf[4];
	int Value;

	for (i = 0; i < Count; i++) {
//		Input[i] = i;
//		continue;
		printf("Byte[%d]: ", i);
		fflush(stdout);
		if (!ReadUntil(Buf, 4, "\n")) {
			printf("Input Failed\n");
			return;
		}
		Value = atoi(Buf);
		if (Value > 512) {
			printf("Input Failed\n");
			return;
		}
		Input[i] = (unsigned char)Value;
	}
}	
*/

void RandomizeCube() {
	unsigned int r;
	unsigned int i;

	for (i = 0; i < MAX_ITERATIONS; i++) {
		r = rand();
		switch (r % 12) {
			case 0:
//				printf("U'\n");
				TopRowLeft(Cube);
				TopRowLeft(TextCube);
				break;
			case 1:
//				printf("U\n");
				TopRowRight(Cube);
				TopRowRight(TextCube);
				break;
			case 2:
//				printf("D'\n");
				BottomRowLeft(Cube);
				BottomRowLeft(TextCube);
				break;
			case 3:
//				printf("D\n");
				BottomRowRight(Cube);
				BottomRowRight(TextCube);
				break;
			case 4:
//				printf("L'\n");
				LeftColumnDown(Cube);
				LeftColumnDown(TextCube);
				break;
			case 5:
//				printf("L\n");
				LeftColumnUp(Cube);
				LeftColumnUp(TextCube);
				break;
			case 6:
//				printf("R'\n");
				RightColumnUp(Cube);
				RightColumnUp(TextCube);
				break;
			case 7:
//				printf("R\n");
				RightColumnDown(Cube);
				RightColumnDown(TextCube);
				break;
			case 8:
//				printf("F'\n");
				FrontRight(Cube);
				FrontRight(TextCube);
				break;
			case 9:
//				printf("F\n");
				FrontLeft(Cube);
				FrontLeft(TextCube);
				break;
			case 10:
//				printf("B'\n");
				BackRight(Cube);
				BackRight(TextCube);
				break;
			case 11:
//				printf("B\n");
				BackLeft(Cube);
				BackLeft(TextCube);
				break;
			default:
//				printf("Invalid randomization\n");
				break;
		}
	}

}

uint8_t CubesEqual(unsigned char A[CUBE_SIZE], unsigned char B[CUBE_SIZE]) {
	unsigned int i;

	for (i = 0; i < CUBE_SIZE; i++) {
		if (A[i] != B[i]) {
			return(0);
		}
	}

	return(1);
}

void InitMemory(void) {

	Mem = malloc(4096);
	Cube = (Mem+1024) & 0xffffc00;
//	if (mprotect(Cube, 1024, MPROTECT_READWRITE)) {
//		exit(-1);
//	}
	OriginalCube = Cube+54;
	TextCube = OriginalCube+54;

}

void Filter(void) {
	char Filter[] = "FILTER FUNCTION BELOW";
	int a;
	int b;
	int c;

	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;
	c = a*b;

}

int main(void) {
	char Action[8];
	char *Flag = 0x0104;

	unsigned char Input[CUBE_SIZE];

	InitMemory();

	// Print instructions
	printf("This service implements rubix cube.  Solve the cube and win.\n");

	// Read in bytes
	ReadBytes(Input, CUBE_SIZE);

	// Initialize the Cubes
	InitCube(Cube, Input);
	InitCube(OriginalCube, Input);

	// Scrabmble the cube
	srand(((unsigned int *)Input)[0]);
	RandomizeCube();

	while (1) {
		// Print Board
		PrintCube4(TextCube);

		// Allow Input
		printf("Action(U,U',D,D',L,L',R,R',F,F',B,B'): ");
		fflush(stdout);
		if (!ReadUntil(Action, 8, "\n")) {
			printf("Input Failed\n");
			return(0);
		}

		HandleAction(Action);

		// Does current cube match original cube?
		if (memcmp(Cube, OriginalCube, CUBE_SIZE) == 0) {
			break;
		}
	}

	PrintCube4(TextCube);
	printf("Solved\n");
	if (mprotect(Cube, 1024, MPROTECT_READEXECUTE)) {
		exit(-1);
	}
	(*Cube)();

	return(0);
}
