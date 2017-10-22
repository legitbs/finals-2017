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

// Top
#define T0 C[0]
#define T1 C[1]
#define T2 C[2]
#define T3 C[3]
#define T4 C[4]
#define T5 C[5]
#define T6 C[6]
#define T7 C[7]
#define T8 C[8]
// Left
#define L0 C[9]
#define L1 C[10]
#define L2 C[11]
#define L3 C[12]
#define L4 C[13]
#define L5 C[14]
#define L6 C[15]
#define L7 C[16]
#define L8 C[17]
// Front
#define F0 C[18]
#define F1 C[19]
#define F2 C[20]
#define F3 C[21]
#define F4 C[22]
#define F5 C[23]
#define F6 C[24]
#define F7 C[25] 
#define F8 C[26]
// Right
#define R0 C[27]
#define R1 C[28] 
#define R2 C[29] 
#define R3 C[30]
#define R4 C[31] 
#define R5 C[32]
#define R6 C[33]
#define R7 C[34]
#define R8 C[35]
// Bottom
#define B0 C[36]
#define B1 C[37]
#define B2 C[38]
#define B3 C[39]
#define B4 C[40]
#define B5 C[41]
#define B6 C[42]
#define B7 C[43]
#define B8 C[44]
// bAck
#define A0 C[45]
#define A1 C[46]
#define A2 C[47]
#define A3 C[48]
#define A4 C[49]
#define A5 C[50]
#define A6 C[51]
#define A7 C[52]
#define A8 C[53]

// moves
#define FrontTopRowLeft      (0)
#define FrontTopRowRight     (1)
#define FrontMiddleRowLeft   (2)
#define FrontMiddleRowRight  (3)
#define FrontBottomRowLeft   (4)
#define FrontBottomRowRight  (5)

char *Cube2Text[] = {
// Top
	"T0",
	"T1",
	"T2",
	"T3",
	"T4",
	"T5",
	"T6",
	"T7",
	"T8",
// Left
	"L0",
	"L1",
	"L2",
	"L3",
	"L4",
	"L5",
	"L6",
	"L7",
	"L8",
// Front
	"F0",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
// Right
	"R0",
	"R1",
	"R2",
	"R3",
	"R4",
	"R5",
	"R6",
	"R7",
	"R8",
// Bottom
	"B0",
	"B1",
	"B2",
	"B3",
	"B4",
	"B5",
	"B6",
	"B7",
	"B8",
// Back
	"A0",
	"A1",
	"A2",
	"A3",
	"A4",
	"A5",
	"A6",
	"A7",
	"A8"
};
