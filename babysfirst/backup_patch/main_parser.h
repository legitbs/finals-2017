/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: main_parser.lib
*
* Description:
*		Contains the main parser functions such as the front end processor as well
* as control structure handlers (loop declaration handler, function handler,
* etc).
*
* Revision:
*		JW 6/15/04 - 1.0
*
*  	7-11 		P. Nguyen 		Added:
*                              SC_ENDOFSCOPE
*                              DoWHILEHandler
*                              SkipCaseStatement
*                              DoFORHandler
*
*		7-12 		P. Nguyen 		Added:
*										 pServer->cForLoopCounter = 1 at PHPCodeHandler
*										 and DoBlankExpressionCall
********************************************************************************
*******************************************************************************/
#include "my_http.h"
#include "tokenizer.h"

#ifndef auto
#define auto
#endif

#ifndef NULL
#define NULL	(0)
#endif

// Enable to debug main parser
//#define MPARSER_DEBUG

/*** BeginHeader */



/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_MPARSER_DEBUG
	#define _mparser_nodebug debug
	#define _mparser_assert(exp) assert(exp)
#else
	#define _mparser_nodebug nodebug
	#define _mparser_assert(exp)			// Define to nothing
#endif

#define SC_OK							0  // Everything was ok
#define SC_END_OF_FILE_FOUND		1	// End of file found during parsing of a structure.
#define SC_SYNTAX_ERROR				2  // Improper syntax for a declaration
#define SC_DIVIDEBYZERO_ERROR		3  // Divide by zero
#define SC_OUTOFMEMORY				4 	// Ran out of memory
#define SC_INTERNALERROR			5  // Internal error while processing
#define SC_ENDOFSCOPE  				6  // Current scope ended prematurely
#define SC_RUNTIME_ERROR			8	// Runtime error

#define EXP_OPERATOR_STACK_SIZE		10
#define EXP_OPERAND_STACK_SIZE		15
#define EXP_FUNCARG_SIZE				10

/* Define the types of operators for an expression */
#define EXP_OPTYPE_PREFIX			0
#define EXP_OPTYPE_POSTFIX			1
#define EXP_OPTYPE_INFIX			2
#define EXP_OPTYPE_CONFIX_OPEN   3
#define EXP_OPTYPE_CONFIX_CLOSE	4
#define EXP_OPTYPE_FUNC_OPEN		5
#define EXP_OPTYPE_FUNC_CLOSE		6
#define EXP_OPTYPE_NOTFOUND		7

/* Jump  back entry types */
#define JBETYPE_SCOPE		1	// Jump back when a closing scope is found
#define JBETYPE_STATEMENT	2  // Jump back at the end of a single statement

#define JBETYPE_SWITCH		8	// Jump back entry is a for a switch statement (of course they don't actually jump back).
#define JBETYPE_DOWHILE		16
#define JBETYPE_LOOP			32	// Jump back entry is for a loop statement
#define JBETYPE_COND			64	// Jump back entry is for a conditional statement
#define JBETYPE_FUNCTION  128 // Jump back entry is for a function

/* Define the states for the current expression parser - used for error checking */
#define EXP_STATE_PRE			1
#define EXP_STATE_POST			2
#define EXP_STATE_ERROR			3

#define EXP_ACTION_SHIFT					1
#define EXP_ACTION_REDUCE		  			2
#define EXP_ACTION_PRECEDENCE		  		3
#define EXP_ACTION_PRECEDENCE_ASSOC		4
#define EXP_ACTION_ERROR					5

// Define the associativities
#define EXP_ASSOC_LEFT		1
#define EXP_ASSOC_RIGHT		2
#define EXP_ASSOC_NONE		3

#define EAS			1  		// Short defines for the previous defines.
#define EAR			2
#define EAP			3
#define EAA			4
#define ERR			5

#define SSTATE_INSTRING	100

const int ExpressionParserTable[7][7] = {

/* Prefix         */		{ EAS, EAP, EAP, EAS, EAR, EAP, EAR },
/* Postfix        */		{ ERR, EAR, EAR, ERR, EAR, EAR, EAR },
/* Infix          */		{ EAS, EAP, EAA, EAS, EAR, EAP, EAR },
/* Confix Open    */    { EAS, EAS, EAS, EAS, EAS, EAS, EAR },
/* Confix Close	*/    { EAR, EAR, EAR, EAR, EAR, EAR, EAR },
/* Func Open      */    { EAS, EAS, EAS, EAS, EAS, EAS, EAR },
/* Func Close     */    { EAR, EAR, EAR, EAR, EAR, EAR, EAR }

};

#undef EAS
#undef EAR
#undef EAP
#undef EAA



/*** EndHeader */



/*** BeginHeader FrontEndProcessor */
int FrontEndProcessor( HttpServerStruct *pServer );
int PHPCodeHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoFunctionHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoEchoHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoBlankExpressionCall( HttpServerStruct *pServer, TokenStruct *pToken );
int DoBreakHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoEndOfScope( HttpServerStruct *pServer, TokenStruct *pToken );
int SkipToEndOfScope( HttpServerStruct *pServer, int iScope );
int DoIFHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoDOWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int CheckDOWHILELoop( HttpServerStruct *pServer, TokenStruct *pToken, int *bTakeLoop );
int DoFORHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int DoSWITCHHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int SkipToFirstOccurSymbol( HttpServerStruct *pServer, TokenStruct *pToken, TokenStruct *pToken, unsigned int ucSymbol );
int SkipCaseStatement( HttpServerStruct *pServer, TokenStruct * pToken );
int DoCaseHandler( HttpServerStruct *pServer, TokenStruct *pToken );
int SkipStatementOrCodeBlock( HttpServerStruct *pServer, TokenStruct *pToken );
int DoExpressionParser( HttpServerStruct *pServer, PHPVariable *pReturn, unsigned int ucEndSymbol );
int DoExpressionReduce( HttpServerStruct *pServer, PHPOperator *pOperatorStack, PHPVariable **pOperandStack, int *cOperatorStackTop, int *cOperandStackTop, int iAllocatedOperandStack[] );
int ConvertVariableToBoolean( PHPVariable *pVar );
int ConvertVariableToInt( PHPVariable *pVar );
//float ConvertVariableToFloat( PHPVariable *pVar );
int ConvertVariableToString( PHPVariable *pVar, char *pDest );
int GetOperatorType( unsigned int ucSymbol, int cExpState );
int GetOperatorPrecedence( unsigned int ucSymbol );
int GetOperatorAssociativity( unsigned int ucSymbol );
void CopyPHPVariable( HttpServerStruct *pServer, PHPVariable *pDest, PHPVariable *pSrc );
int DoNormalComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal );
int DoStrictComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal );
void DoLessThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal );
void DoGreaterThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal );
void DoXORComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal );
