/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: php_functions.lib
*
* Description:
*		Contains all the PHP Functions that are predefined and can be used in
* the php code.
*
* Revision:
*		JW 7/10/04 - 1.0
*
********************************************************************************
*******************************************************************************/

#include "php_lib1.h"
#include "php_lib2.h"
#include "php_libspc.h"

/*** BeginHeader */

/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_FUNCTION_DEBUG
	#define _function_nodebug debug
	#define _function_assert(exp) assert(exp)
#else
	#define _function_nodebug nodebug
	#define _function_assert(exp)			// Define to nothing
#endif

typedef struct {
	char pFuncName[40];
   int iFuncNameLen;

   char (*pFunctionHandler)( );

   unsigned char iArgCount;
} PHPInternalFunction;

typedef struct {
	char pFuncName[40];
   int iFuncNameLen;

   char (*pFunctionHandler)( );
} PHPSpecialFunction;

const PHPInternalFunction g_oInternalFunctionList[] = {
{ 	  		"test", 	4, 		test_function, 	2 },
{			 "abs",	3,			 abs_function,		1 },
{ 	  	"pow", 		3, 		pow_function, 			2 },
{ 	  	"srand", 	5, 		test_function, 		1 },
{ 	  	"rand", 		4, 		rand_function, 		0 },
{ 	  	"strlen", 	6, 		strlen_function, 		1 },
{ 	  	"strcmp", 	6, 		strcmp_function, 		2 },
{ 	  	"strncmp", 	7, 		strncmp_function,		3 },
{ 	  	"getdate",	7, 		getdate_function,		0 },
{ 	  	"setdatetime",	11, 		setdatetime_function,		6 },
{ 	  	"strtolower",	10, 		strtolower_function,		1 },
{ 	  	"strtoupper",	10, 		strtoupper_function,		1 },
{     "setcookie", 9,		setcookie_function,	2 },
{ 	  	"is_int",			6, 		is_int_function,				1 },
{ 	  	"is_float",			8, 		is_float_function,			1 },
{ 	  	"is_string",		9, 		is_string_function,			1 },
{ 	  	"trim",				4, 		trim_function,					1 },
{ 	  	"ltrim",				5, 		ltrim_function,				1 },
{ 	  	"rtrim",				5, 		rtrim_function,				1 },
{ 	  	"strstr",			6, 	   strstr_function,				2 },
{ 	  	"strchr",			6, 	   strstr_function,				2 },
{ 	  	"strcasecmp",		10, 		strcasecmp_function,			2 },
{	   "uploadfile",		10,		uploadfile_function,			2 },
{ 				 "", 	0, 					NULL, 	0 }
};


const PHPInternalFunction g_oSpecialFunctionList[] = {
{ 	  	  "_POST", 	5, 		    post_data_spcfunc },
{		   "_GET",  4,            get_data_spcfunc },
{		"_COOKIE",  7,			  cookie_data_spcfunc },
{				 "",  0,					           NULL }
};


/*** EndHeader */


/*** BeginHeader FindInternalFunction */
PHPInternalFunction *FindInternalFunction( HttpServerStruct *pServer, char *pName, int iNameLen, int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
FindInternalFunction                   <PHP_FUNCTIONS.LIB>

SYNTAX: PHPInternalFunction *FindInternalFunction( HttpServerStruct *pServer, char *pName, int iNameLen, int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.
PARAMETER2: The name of the function
PARAMETER3: The length of the name of the function
PARAMETER4: The argument count for the function

RETURN VALUE:  Returns the data structure corresponding to the function,
					and NULL if it is not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_function_nodebug PHPInternalFunction *FindInternalFunction( HttpServerStruct *pServer, char *pName, int iNameLen, int iArgCount )
{
   auto int iCur;

   for ( iCur = 0; ; iCur++ )
   {
   	if ( g_oInternalFunctionList[iCur].iFuncNameLen == 0 )
      	break;

      if ( g_oInternalFunctionList[iCur].iFuncNameLen == iNameLen &&
      	  g_oInternalFunctionList[iCur].iArgCount == iArgCount &&
           strncmp( g_oInternalFunctionList[iCur].pFuncName, pName, iNameLen ) == 0 )
      {
      	return (&g_oInternalFunctionList[iCur]);
      }
   }

   return (NULL);
}


/*** BeginHeader CallFunction */
char CallFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], char *pName, int iNameLen, int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
CallFunction                   <PHP_FUNCTIONS.LIB>

SYNTAX: char CallFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], char *pName, int iNameLen, int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.
PAREMETER2: The return value from the function
PAREMETER3: The argument list for the function
PARAMETER4: The name of the function
PARAMETER5: The length of the name of the function
PARAMETER6: The argument count for the function

RETURN VALUE:  Returns the data structure corresponding to the function,
					and NULL if it is not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_function_nodebug char CallFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], char *pName, int iNameLen, int iArgCount )
{
   auto PHPInternalFunction *pInternalFunc;

   if ( (pInternalFunc = FindInternalFunction( pServer, pName, iNameLen, iArgCount )) != NULL )
   {
   	return (*pInternalFunc->pFunctionHandler)( pServer, pReturn, iArgList, iArgCount );
   }


   return (7); // Function not found
}


/*** BeginHeader FindSpecialFunction */
PHPSpecialFunction *FindSpecialFunction( HttpServerStruct *pServer, char *pName, int iNameLen );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
FindSpecialFunction                   <PHP_FUNCTIONS.LIB>

SYNTAX: PHPSpecialFunction *FindSpecialFunction( HttpServerStruct *pServer, char *pName, int iNameLen );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.
PARAMETER2: The name of the function
PARAMETER3: The length of the name of the function

RETURN VALUE:  Returns the data structure corresponding to the function,
					and NULL if it is not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_function_nodebug PHPSpecialFunction *FindSpecialFunction( HttpServerStruct *pServer, char *pName, int iNameLen )
{
   auto int iCur;

   for ( iCur = 0; ; iCur++ )
   {
   	if ( g_oSpecialFunctionList[iCur].iFuncNameLen == 0 )
      	break;

      if ( g_oSpecialFunctionList[iCur].iFuncNameLen == iNameLen &&
           strncmp( g_oSpecialFunctionList[iCur].pFuncName, pName, iNameLen ) == 0 )
      {
      	return (&g_oSpecialFunctionList[iCur]);
      }
   }

   return (NULL);
}


/*** BeginHeader CallSpecialFunction */
char CallSpecialFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg, char *pName, int iNameLen );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
CallSpecialFunction                   <PHP_FUNCTIONS.LIB>

SYNTAX: char CallSpecialFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg, char *pName, int iNameLen );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.
PAREMETER2: The return value from the function
PAREMETER3: The single argument for the functino
PARAMETER4: The name of the function
PARAMETER5: The length of the name of the function

RETURN VALUE:  Returns the data structure corresponding to the function,
					and NULL if it is not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_function_nodebug char CallSpecialFunction( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg, char *pName, int iNameLen )
{
   auto PHPSpecialFunction *pSpecialFunc;

   if ( (pSpecialFunc = FindSpecialFunction( pServer, pName, iNameLen )) != NULL )
   {
   	return (*pSpecialFunc->pFunctionHandler)( pServer, pReturn, iArg );
   }


   return (7); // Function not found
}
