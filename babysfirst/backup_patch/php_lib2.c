/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: php_lib2.lib
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

#include "php_lib2.h"
#include "php_vars.h"
#include <tokenizer.h>


/*** BeginHeader */


/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_FUNCTION_DEBUG
	#define _phplib2_nodebug debug
	#define _phplib2_assert(exp) assert(exp)
#else
	#define _phplib2_nodebug nodebug
	#define _phplib2_assert(exp)			// Define to nothing
#endif




int abs_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
abs_function                   <PHP_LIB2.LIB>

SYNTAX: int abs_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:	PHP Absolute value function.

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib2_nodebug int abs_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	PHPVariable *pArg1;

   pArg1 = &(pServer->php_interpreter.oVariableTable[iArgList[0]]);

   if ( pArg1->cVarType == VARTYPE_INT )
   {
   	pReturn->VarData.iIntValue = labs( pArg1->VarData.iIntValue );
      pReturn->cVarType = VARTYPE_INT;
   }
#if 0
   else if ( pArg1->cVarType == VARTYPE_FLOAT )
   {
   	pReturn->VarData.fFloatValue = fabs( pArg1->VarData.fFloatValue );
      pReturn->cVarType = VARTYPE_FLOAT;
   }
#endif
   else // Any other data type we treat as an integer - by converting it to an int first.
   {
      pReturn->VarData.iIntValue = labs( ConvertVariableToInt( pArg1 ) );
   	pReturn->cVarType = VARTYPE_INT;
   }


   return 0;
}
