/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
*		Phuoc Nguyven		(alias PN)
* 		Jason Williams		(alias JW)
*		Duong Nguyven		(alias DN)
*
*
* File Name: tokenizer.lib
*
* Description:
*		PN 6/13/04 - Parse Data into tokens
* Revision:
*     7-11 	P. Nguyen		added default token
*
********************************************************************************
*******************************************************************************/

#include "my_http.h"

/*** BeginHeader */

#ifdef RC_TOKEN_DEBUG
	#define _token_nodebug debug
	#define _token_assert(exp) assert(exp)
#else
	#define _token_nodebug nodebug
	#define _token_assert(exp)			// Define to nothing
#endif

/******************************************************************************
Global Defines
******************************************************************************/

#define TRUE							1
#define FALSE							0


#define TOKEN_ASSIGNMENT            1
#define TOKEN_BREAK                 2
#define TOKEN_CONTINUE              3
#define TOKEN_DECREMENT             4
#define TOKEN_END                   5
#define TOKEN_EQUALTO               6
#define TOKEN_FOR                   7
#define TOKEN_FUNCTION              8
#define TOKEN_GLOBAL                9
#define TOKEN_GREATER_THAN          10
#define TOKEN_GREATER_EQUAL         12
#define TOKEN_IF                    13
#define TOKEN_ELSEIF                14
#define TOKEN_ELSE                  15
#define TOKEN_INCREMENT             16
#define TOKEN_LESS_THAN             17
#define TOKEN_LESS_EQUAL            18
#define TOKEN_NEWLINE               19
#define TOKEN_NEXT                  20
#define TOKEN_NOT_EQUAL             21
#define TOKEN_PHP_CLOSE_TAG         22
#define TOKEN_RETURN                23
#define TOKEN_RSORT                 24
#define TOKEN_STATIC                25
#define TOKEN_SWITCH                26
#define TOKEN_WHILE                 27
#define TOKEN_OPEN_PAREN            28
#define TOKEN_CLOSE_PAREN           29
#define TOKEN_INT                   30
#define TOKEN_SEMICOLON             31
#define TOKEN_CHAR                  32
#define TOKEN_NOT                   33
#define TOKEN_ADD                   34
#define TOKEN_SUBTRACT              35
#define TOKEN_MOD                   36
#define TOKEN_AMPERSAND             37
#define TOKEN_STAR                  38
#define TOKEN_LOGICAL_AND           39
#define TOKEN_LOGICAL_OR            40
#define TOKEN_OPEN_SQUIGGLY         41
#define TOKEN_CLOSE_SQUIGGLY        42
#define TOKEN_OPEN_BRACKET          43
#define TOKEN_CLOSE_BRACKET         44
#define TOKEN_PERIOD                45
#define TOKEN_COMMA                 46
#define TOKEN_FORWARD_SLASH         47
#define TOKEN_COLON                 48
#define TOKEN_DOUBLE_SLASH          49
#define TOKEN_OPEN_COMMENT          50
#define TOKEN_CLOSE_COMMENT         51
#define TOKEN_DO                    52
#define TOKEN_FOR_EACH              53
#define TOKEN_NOT_EQUAL_IDENT       54
#define TOKEN_LESS_EQUAL_IDENT      55
#define TOKEN_EQUAL_IDENT           56
#define TOKEN_QUESTION_MARK         57
#define TOKEN_CASE                  58
#define TOKEN_ECHO                  59
#define TOKEN_FLOAT                 60
#define TOKEN_STRING                61
#define TOKEN_XOR                   62
#define TOKEN_TILDA                 63
#define TOKEN_SHIFT_LEFT_EQUAL      64
#define TOKEN_SHIFT_RIGHT_EQUAL     65
#define TOKEN_BOOLEAN               66
#define TOKEN_CAST_BOOLEAN          67
#define TOKEN_CAST_CHAR             68
#define TOKEN_CAST_INT              69
#define TOKEN_CAST_FLOAT            70
#define TOKEN_CAST_STRING           71
#define TOKEN_STAR_EQUAL            72
#define TOKEN_ADD_EQUAL             73
#define TOKEN_SUBTRACT_EQUAL        74
#define TOKEN_FORWARD_SLASH_EQUAL   75
#define TOKEN_DEFAULT					76
#define TOKEN_MOD_EQUAL					77
#define TOKEN_PERIOD_EQUAL				78
#define TOKEN_AMPERSAND_EQUAL			79
#define TOKEN_OR_EQUAL					80
#define TOKEN_XOR_EQUAL					81
#define TOKEN_TILDA_EQUAL  			82
#define TOKEN_SHIFT_LEFT            83
#define TOKEN_SHIFT_RIGHT           84
#define TOKEN_LOGICAL_AND2				85
#define TOKEN_LOGICAL_OR2				86
#define TOKEN_LOGICAL_XOR2				87


#define TYPE_SYMBOL             1
#define TYPE_LONG               2
#define TYPE_FLOAT              3
#define TYPE_IDENTIFIER         4
#define TYPE_STRING             5
#define TYPE_CHAR               6
#define TYPE_VARIABLE           7

#define LIST_EOF                255
#define ERROR                   -1
#define NOT_FOUND               -2
#define ERROR_EOF               -3



/******************************************************************************
Name:			SymbolTable
Description:	struct to store list of token id
******************************************************************************/

typedef struct
{
	char symbolString[15];
	unsigned int ucSymbolValue;
	unsigned int ucSymbolStringLen;
} SymbolTable;

/******************************************************************************
Name: TokenStruct
Description: Hold data for a single token.
******************************************************************************/
typedef struct
{
   unsigned int ucTokenType;
   char sStringBuffer[PHP_MAX_STRINGLENGTH];

   /* TODO: Added by Jason Williams to get the location of a token in a file. */
   char *ppTokenLocation;

	union
	{
		unsigned int ucIdentifierLen;	/* length of ident. string */
		unsigned int ucStringLen;
		unsigned int ucSymbolValue;
		char cCharValue;
		int lValue;					/* token is an integer constant */
	} TokenData;

} TokenStruct;

/*** BeginHeader Tokenizer_Init */
int Tokenizer_Init();
int GotoFirstNonSpace( HttpServerStruct *pServer );
int ConvertStringToNum( int * lNum, HttpServerStruct *pServer );
int IsEndingTokenChar(int checkChar);
int GotoEndingTokenChar( HttpServerStruct *pServer ) ;
int GotoFirstOccurChar( HttpServerStruct *pServer, int char1 );
int CompareTokens( char *pSymbol, unsigned int ucStart, unsigned int ucEnd, TokenStruct *T );
int BinSearchSymbol( TokenStruct * T, HttpServerStruct *pServer );
int GetCurrentToken ( TokenStruct *T, HttpServerStruct *pServer );
int PHPTextParser( HttpServerStruct *pServer );
int TestInterpreter( HttpServerStruct *pServer );

