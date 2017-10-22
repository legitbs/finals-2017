/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: php_vars.lib
*
* Description:
*		Contains the functions necessary to create PHP variables and access there
* data members.
*
* Revision:
*		JW 6/15/04 - 1.0
*
********************************************************************************
*******************************************************************************/

#include "my_http.h"

/*** BeginHeader */

/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_PHPVAR_DEBUG
	#define _phpvar_nodebug debug
	#define _phpvar_assert(exp) assert(exp)
#else
	#define _phpvar_nodebug nodebug
	#define _phpvar_assert(exp)			// Define to nothing
#endif

#define VARTYPE_BOOLEAN	0
#define VARTYPE_INT		1
#define VARTYPE_FLOAT	2
#define VARTYPE_STRING	3

// A temporary var is used here -- don't use the variable table
#define VAR_VNUM_TEMPORARY	(-1)

/*** EndHeader */



/*** BeginHeader InitVarTable */
int InitVarTable( HttpServerStruct *pServer, int iSize );
int CreateVariable( HttpServerStruct *pServer );
int FreeVariable( HttpServerStruct *pServer, int iCurVar );
int FindVariable( HttpServerStruct *pServer, char *pName, int iLength, int iFunctionScope );
int FindOrCreateVariable( HttpServerStruct *pServer, char *pName, int iLength );
int SaveVariableString( HttpServerStruct *pServer, PHPVariable *pVar, char *pString, int iLength );
int LoadVariableString( PHPVariable *pVar, char *pString, int iLength );
int FreeVariablesInScope( HttpServerStruct *pServer, int iScope );
