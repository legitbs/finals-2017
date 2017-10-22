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
#include "php_vars.h"
#include "tokenizer.h"
#include "out_helper.h"

/* START FUNCTION DESCRIPTION ********************************************
InitVarTable                   <MAIN_PARSER.LIB>

SYNTAX: int InitVarTable( HttpServerStruct *pServer );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns TRUE, if there were no errors while processing, FALSE,
		         otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int InitVarTable( HttpServerStruct *pServer, int iSize )
{
	auto PHPVariable *pVarTable;
   auto int iCur;

   pVarTable = (pServer->php_interpreter.oVariableTable);

   pServer->php_interpreter.iVarTableSize = iSize;
   pServer->php_interpreter.iFreeList = -1;
   pServer->php_interpreter.iVarList = -1;

   for ( iCur = 0; iCur < (iSize-1); iCur++ )
   {
   	pVarTable[iCur].iNext = (iCur + 1);
   }

   pVarTable[iCur].iNext = -1; // End of list.

   pServer->php_interpreter.iFreeList = 0;
}


/* START FUNCTION DESCRIPTION ********************************************
CreateVariable                   <MAIN_PARSER.LIB>

SYNTAX: int CreateVariable( HttpServerStruct *pServer );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns -1 if there was an error while processing, a positive
					integer otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int CreateVariable( HttpServerStruct *pServer )
{
	// Grab a variable from the free list and place it into the
   // variable table and initialize the variable to zero.
   auto int iFreeCur;
   auto PHPVariable *pVarTable;
   auto int iCur;

   pVarTable = (pServer->php_interpreter.oVariableTable);
	iFreeCur = (pServer->php_interpreter.iFreeList);

   if ( iFreeCur == -1 )
   {
   	return -1; // No more variable space available, free list empty.
   }

   // Move to the next element in the free list
   pServer->php_interpreter.iFreeList = pVarTable[iFreeCur].iNext;

   // Link the next element to the beginning of the var list.
   pVarTable[iFreeCur].iNext = pServer->php_interpreter.iVarList;
   pServer->php_interpreter.iVarList = iFreeCur;

   pVarTable[iFreeCur].szVarName[0] = '\0';
   pVarTable[iFreeCur].iVarNameLen = 0;

   pVarTable[iFreeCur].iVNum = iFreeCur;
   pVarTable[iFreeCur].iScope = pServer->php_interpreter.iCurrentScope;

   pVarTable[iFreeCur].cVarType = VARTYPE_INT;
   pVarTable[iFreeCur].VarData.iIntValue = 0;

   return (iFreeCur);
}



/* START FUNCTION DESCRIPTION ********************************************
FreeVariable                   <MAIN_PARSER.LIB>

SYNTAX: int FreeVariable( HttpServerStruct *pServer );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns FALSE if there was an error while processing, TRUE
					otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int FreeVariable( HttpServerStruct *pServer, int iCurVar )
{
   // Place this variable into the free list, unlink it from the current
   // variable list (this requires scanning the list).
   auto int iFreeCur;
   auto PHPVariable *pVarTable;
   auto int iCur;
   auto int iSize;

   pVarTable = (pServer->php_interpreter.oVariableTable);
	iFreeCur = (pServer->php_interpreter.iFreeList);
   iSize = (pServer->php_interpreter.iVarTableSize);

   if ( pServer->php_interpreter.iVarList == iCurVar )
   {
   	pServer->php_interpreter.iVarList = pVarTable[iCurVar].iNext;

      pVarTable[iCurVar].iNext = iFreeCur;
      pServer->php_interpreter.iFreeList = iCurVar;
   }
   else
   {
  		// Scan the variable list to remove this element
   	for ( iCur = pServer->php_interpreter.iVarList; iCur != -1; iCur = pVarTable[iCur].iNext )
   	{
      	if ( pVarTable[iCur].iNext == iCurVar )
         {
         	pVarTable[iCur].iNext = pVarTable[iCurVar].iNext;

            break;
         }
   	}

      pVarTable[iCurVar].iNext = iFreeCur;
      pServer->php_interpreter.iFreeList = iCurVar;
   }

   return (TRUE);
}


/* START FUNCTION DESCRIPTION ********************************************
FindVariable                   <MAIN_PARSER.LIB>

SYNTAX: int FindVariable( HttpServerStruct *pServer, char *pName, int iLength, int iFunctionScope );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the index into the variable table for the variable or -1
					if a variable cannot be found and/or created.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int FindVariable( HttpServerStruct *pServer, char *pName, int iLength, int iFunctionScope )
{
	auto int iCur;
   auto PHPVariable *pVarTable;

   pVarTable = pServer->php_interpreter.oVariableTable;

   iCur = pServer->php_interpreter.iVarList;

   for (;;)
   {
   	if ( iCur == -1 )
      {
      	// End of list. break;
         break;
      }

      if ( pVarTable[iCur].iScope < iFunctionScope )
      	continue;

      if ( pVarTable[iCur].iVarNameLen == iLength &&
      	  strncmp( pVarTable[iCur].szVarName, pName, iLength ) == 0 )
      {
      	// Match found return the index.
         return iCur;
      }

      iCur = pVarTable[iCur].iNext;
   }

   return (-1); // Not found
}


/* START FUNCTION DESCRIPTION ********************************************
FindOrCreateVariable                   <MAIN_PARSER.LIB>

SYNTAX: int FindOrCreateVariable( HttpServerStruct *pServer, char *pName, int iLength );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the index into the variable table for the variable or -1
					if a variable cannot be found and/or created.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int FindOrCreateVariable( HttpServerStruct *pServer, char *pName, int iLength )
{
	auto int iVar;

   if ( (iVar = FindVariable( pServer, pName, iLength, pServer->php_interpreter.iFunctionScope )) != -1 )
   	return iVar;

   // Not found - thus create variable
   if ( (iVar = CreateVariable( pServer )) == -1 )
   	return (-1); // Error no more variable space available to create

   // Ok we have a variable to create - setup the name and name length of the variable.
   strncpy( pServer->php_interpreter.oVariableTable[iVar].szVarName, pName, iLength );
   pServer->php_interpreter.oVariableTable[iVar].iVarNameLen = iLength;

   return iVar;
}


/* START FUNCTION DESCRIPTION ********************************************
SaveVariableString                   <MAIN_PARSER.LIB>

SYNTAX: int SaveVariableString( HttpServerStruct *pServer, PHPVariable *pVar, char *pString, int iLength );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns FALSE if there was an error while processing, TRUE
					otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int SaveVariableString( HttpServerStruct *pServer, PHPVariable *pVar, char *pString, int iLength )
{
	pVar->cVarType = VARTYPE_STRING;

   if ( pVar->iVNum != VAR_VNUM_TEMPORARY )
   	pVar->VarData.ppStringValue = (pServer->php_interpreter.ppStringVarBufLoc + (pVar->iVNum * PHP_MAX_STRINGLENGTH));

   if ( root2xmem( pVar->VarData.ppStringValue, pString, iLength ) < 0 )
   	return FALSE;

   return TRUE;
}


/* START FUNCTION DESCRIPTION ********************************************
LoadVariableString                   <MAIN_PARSER.LIB>

SYNTAX: int LoadVariableString( PHPVariable *pVar, char *pString, int iLength );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns FALSE if there was an error while processing, TRUE
					otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int LoadVariableString( PHPVariable *pVar, char *pString, int iLength )
{
	if ( pVar->cVarType != VARTYPE_STRING )
   	return FALSE;

   xmem2root( pString, pVar->VarData.ppStringValue, iLength );

   return TRUE;
}


/* START FUNCTION DESCRIPTION ********************************************
FreeVariablesInScope                   <MAIN_PARSER.LIB>

SYNTAX: int FreeVariablesInScope( HttpServerStruct *pServer, int iScope )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns FALSE if there was an error while processing, TRUE
					otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phpvar_nodebug int FreeVariablesInScope( HttpServerStruct *pServer, int iScope )
{
   auto int iCur, iNext;
   auto PHPVariable *pVarTable;

   pVarTable = (pServer->php_interpreter.oVariableTable);

   // Scan through the variable list and remove variables with the current scope
   for ( iCur = pServer->php_interpreter.iVarList; iCur != -1; iCur = iNext )
   {
   	iNext = pVarTable[iCur].iNext;

   	if ( pVarTable[iCur].iScope == iScope )
      	FreeVariable( pServer, iCur );
   }
   return (TRUE);
}



