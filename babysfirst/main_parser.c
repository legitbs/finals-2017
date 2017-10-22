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
#include "main_parser.h"
#include "php_vars.h"

#include <string.h>

/* START FUNCTION DESCRIPTION ********************************************
FrontEndProcessor                   <MAIN_PARSER.LIB>

SYNTAX: int FrontEndProcessor( HttpServerStruct *pServer );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns TRUE, if there were no errors while processing, FALSE,
		         otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int FrontEndProcessor( HttpServerStruct *pServer )
{
   TokenStruct T1;
   int status;

   /********************************
   * Call the PHP Interpreter - but
   * first initialize it.
   ********************************/
   pServer->php_interpreter.iTopFunction = -1;
   pServer->php_interpreter.iTopJumpEntry = -1;

   pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
   pServer->php_interpreter.iCurrentScope = 0;
   pServer->php_interpreter.iFunctionScope = -1; // Not in a function
   pServer->php_interpreter.cForLoopCounter = 0;
   pServer->inout_buffer_length = 0;				// Make sure the inout_buffer length is zero since nothing has been written yet.
   InitVarTable( pServer, PHP_MAX_VARIABLETABLE );

   if ( PHPTextParser( pServer ) == FALSE )
   {
   	return TRUE;
   }

   if ( (status = PHPCodeHandler( pServer, &T1 )) != SC_OK )
   {
      // TODO: Check status code
      printf( "\n!!!! SYNTAX ERROR !!!!\n" );

      return FALSE;
   }

   return (TRUE);
}


/* START FUNCTION DESCRIPTION ********************************************
PHPCodeHandler                   <MAIN_PARSER.LIB>

SYNTAX: int PHPCodeHandler( HttpServerStruct *pServer, TokenStruct *pToken );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the status code.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int PHPCodeHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto int status;

#if HTTP_VERBOSE
   printf( "PHPCodeHandler::Called!\n" );
#endif

   while ( GetCurrentToken( pToken, pServer ) == TRUE )
	{
      status = 0;

#if HTTP_VERBOSE
   printf( "PHP Token Type: %d\n", pToken->ucTokenType );
#endif

      switch ( pToken->ucTokenType )
      {
      case TYPE_SYMBOL:
      	// We have received a symbol - check to determine the appropriate actions
         switch( pToken->TokenData.ucSymbolValue )
         {
         /* Check Control Structures *
         if
	  		else
  			elseif
	  		while
  			do..while
  			for
  			foreach
  			break
  			continue
  			switch
  			declare
	  		return
         *****************************/
         case TOKEN_IF:		// if ( <exp> )
         	status = DoIFHandler( pServer, pToken );
            break;

         case TOKEN_WHILE:		// while ( <exp> )
         	status = DoWHILEHandler( pServer, pToken );
            break;

         case TOKEN_DO:			// do { } while ( <exp> )
         	status = DoDOWHILEHandler( pServer, pToken );
            break;

         case TOKEN_FOR:		// for ( <exp>; <exp>; <exp> )
         	status = DoFORHandler( pServer, pToken );
            break;

         case TOKEN_BREAK: 	// break;
         	status = DoBreakHandler( pServer, pToken );
            break;

         case TOKEN_CONTINUE:	// continue;
         	// TODO: DO CONTINUE HANDLER
            break;

         case TOKEN_SWITCH:	// switch ( <var> )
         	status = DoSWITCHHandler( pServer, pToken );
            break;

         case TOKEN_CASE:
         case TOKEN_DEFAULT:
            status = DoCaseHandler( pServer, pToken );
            break;

         /* TODO: ADD DECLARE symbol and write declare handler (what is declare???)
         case TOKEN_DECLARE:
         	// TODO: DO DECLARE HANDLER
            break;
         */

         case TOKEN_RETURN:	// return <exp>;
         	// TODO: DO RETURN HANDLER
            break;


         case TOKEN_FUNCTION:	// function <name>( <args )
         	status = DoFunctionHandler( pServer, pToken );
            break;

         case TOKEN_ECHO:	// echo <exp>;
            status = DoEchoHandler( pServer, pToken );
            break;

         case TOKEN_STATIC: // static <var name>, <var names> ...;
         	// TODO: DO STATIC HANDLER
            break;

         case TOKEN_GLOBAL: // global <var name>, <var names>...;
         	// TODO: DO GLOBAL HANDLER
            break;

         /* TODO: ADD Dollar sign symbol and write dollar sign handler
         case TOKEN_DOLLAR_SIGN: // $<var name>
         	// TODO: DO DOLLAR SIGN HANDLER
            break;
         */

         case TOKEN_OPEN_SQUIGGLY:
            pServer->php_interpreter.iCurrentScope++;
            break;

         case TOKEN_CLOSE_SQUIGGLY:
         	status = DoEndOfScope( pServer, pToken );
				break;

         }

         break; // END SYMBOL

      case TYPE_IDENTIFIER:
      	// Backup to the first token since we had to read ahead one.
      	pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
         MoveSlidingWindow( pServer, TRUE );

      	status = DoBlankExpressionCall( pServer, pToken );
         break;

      case TYPE_VARIABLE:
      	// Backup to the first token since we had to read ahead one.
      	pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
         MoveSlidingWindow( pServer, TRUE );

      	status = DoBlankExpressionCall( pServer, pToken );
         break;

      default:
      	// Unknown token type.
      	return SC_SYNTAX_ERROR;
         break;
      }

      // Check status code
      if ( status != SC_OK )
      {
      	return status; // Pass the buck.
      }
	}

   return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoFunctionHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoFunctionHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoFunctionHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto int status;
   auto PHPFunction *pFunction;
   auto char *ppCodeAddress;

   // Create the function on the function table.
   pServer->php_interpreter.iTopFunction++;
   pFunction = &(pServer->php_interpreter.oFunctionTable[pServer->php_interpreter.iTopFunction]);

   // Save the starting address of the function for use later on.
   pFunction->ppFunctionLocation = pToken->ppTokenLocation;

   if ( GetCurrentToken( pToken, pServer ) == FALSE )
	   return SC_END_OF_FILE_FOUND;

   // Verify syntax
   if ( pToken->ucTokenType != TYPE_IDENTIFIER )
   	return SC_SYNTAX_ERROR;

   // Save the name of the function
   // pFunction->pFuncName = pToken->pTokenLocation;
   pFunction->iFuncNameLen = pToken->TokenData.ucIdentifierLen;

   // Verify syntax
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
	   return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
   	  pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
   	return SC_SYNTAX_ERROR;

   // Read in the argument list for a count of the number of arguments and syntax verification.
   pFunction->iArgLen = 0;
   for (;;)
   {
   	if ( GetCurrentToken( pToken, pServer ) == FALSE )
      	return SC_END_OF_FILE_FOUND;

      if ( pToken->ucTokenType != TYPE_VARIABLE )
      	return SC_SYNTAX_ERROR;

      pFunction->iArgLen++;

      if ( GetCurrentToken( pToken, pServer ) == FALSE )
      	return SC_END_OF_FILE_FOUND;

      if ( pToken->ucTokenType != TYPE_SYMBOL )
      	return SC_SYNTAX_ERROR;

      if ( pToken->TokenData.ucSymbolValue == TOKEN_CLOSE_PAREN )
      	break; // End of function declaration.
      else if ( pToken->TokenData.ucSymbolValue != TOKEN_COMMA )
      	return SC_SYNTAX_ERROR;	// Syntax error.
   }

   // Verify syntax
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
	   return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
   	  pToken->TokenData.ucSymbolValue != TOKEN_OPEN_SQUIGGLY )
   	return SC_SYNTAX_ERROR;

   pServer->php_interpreter.iCurrentScope++;

   if ( (status = SkipToEndOfScope( pServer, pServer->php_interpreter.iCurrentScope )) != SC_OK )
   	return status;

   return SC_OK;
}

/* START FUNCTION DESCRIPTION ********************************************
DoEchoHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoEchoHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoEchoHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto int status;

	char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
	oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
	oReturnValue.VarData.ppStringValue = szReturnTemporary;

#if HTTP_VERBOSE
   printf( "ECHO Handler!\n" );
#endif

   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_SEMICOLON )) != SC_OK )
   	return status;

   if ( pServer->php_interpreter.iTopJumpEntry >= 0 )
   {
   	// Check if the top jump entry is the current scope
      if ( IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_STATEMENT ) )
      {
         // If the jump back type was for a loop then set the loop counter to remember we jumped back for a loop
         if ( IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_LOOP ) )
            pServer->php_interpreter.cForLoopCounter = 1;   // Setting counter if jumpback occured

      	// Take the jump.
      	pServer->php_interpreter.ppFileLocation = pServer->php_interpreter.oJumpBackTable[ pServer->php_interpreter.iTopJumpEntry].ppJumpBackLocation;
         MoveSlidingWindow( pServer, TRUE );

      	pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry since we took it.

         return SC_OK;
      }
   }

   ConvertVariableToString( &oReturnValue, g_oStringBuffer );

   // Write the output
   http_write_output( pServer, g_oStringBuffer, strlen( g_oStringBuffer ) );

   return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoBlankExpressionCall                  <MAIN_PARSER.LIB>

SYNTAX: int DoBlankExpressionCall( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoBlankExpressionCall( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto int status;

	char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_SEMICOLON )) != SC_OK )
   	return status;

   if ( pServer->php_interpreter.iTopJumpEntry >= 0 )
   {
   	// Check if the top jump entry is the current scope
      if ( IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_STATEMENT ) )
      {
         // If the jump back type was for a loop then set the loop counter to remember we jumped back for a loop
         if ( IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_LOOP ) )
            pServer->php_interpreter.cForLoopCounter = 1;   // Setting counter if jumpback occured

      	// Take the jump.
      	pServer->php_interpreter.ppFileLocation = pServer->php_interpreter.oJumpBackTable[ pServer->php_interpreter.iTopJumpEntry].ppJumpBackLocation;
         MoveSlidingWindow( pServer, TRUE );

      	pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry since we took it.

         return SC_OK;
      }
   }

   return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoBreakHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoBreakHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:	Handles "break <optional number>;" in php

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoBreakHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto int status;
   auto int iBreakCount;
   auto int iCurrentJumpEntry;
   auto PHPJumpBackEntry *pJumpEntry;

	char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_SEMICOLON )) != SC_OK )
   	return status;

   // Convert the return value to an integer - to determine how many loops we should break out of.
   iBreakCount = (int)ConvertVariableToInt( &oReturnValue );

   // Check to make sure the count is greater then 0
   if ( iBreakCount <= 0 )
   	iBreakCount = 1;

   // Save the top of the jump back entry stack since we will be traversing down it.
   iCurrentJumpEntry = pServer->php_interpreter.iTopJumpEntry;

   // While there are entries on the jump back entry table.
   while ( iCurrentJumpEntry >= 0 )
   {
   	pJumpEntry = (&pServer->php_interpreter.oJumpBackTable[iCurrentJumpEntry]);

   	if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_LOOP ) )
      {
      	// Don't allow breaks for single statement loops.
      	if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_STATEMENT ) )
         	return SC_SYNTAX_ERROR;

         if ( (status = SkipToEndOfScope( pServer, pJumpEntry->cScope )) != SC_OK )
         	return SC_SYNTAX_ERROR;

         if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_DOWHILE ) )
         {
         	// If it was a do while loop skip over the while ( <exp> ); part that is trailing.
         	if ( (status = SkipStatementOrCodeBlock( pServer, pToken )) != SC_OK )
         		return status;
         }

      	iBreakCount--;

         if ( iBreakCount <= 0 )
         	break;
      }
      else if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_SWITCH ) )
      {
      	// Break out of the switch statement.
      	if ( (status = SkipToEndOfScope( pServer, pJumpEntry->cScope )) != SC_OK )
         	return SC_SYNTAX_ERROR;

         iBreakCount--;

         if ( iBreakCount <= 0 )
         	break;
      }
      else if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_FUNCTION ) )
      {
      	// Error we tried to break out too far (ie we said break 3 and there was only two loops) and we scanned
         // to a function jump back entry.  Send a runtime error.
         return SC_RUNTIME_ERROR;
      }

      iCurrentJumpEntry--;
   }

   if ( iBreakCount > 0 )
   {
   	// Error we were told to break out of too many loops.
   	return SC_RUNTIME_ERROR;
   }

   pServer->php_interpreter.iTopJumpEntry = (iCurrentJumpEntry - 1); // If all went right we exited at the break and we need to subtract one jump entry.

   return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoEndOfScope                 <MAIN_PARSER.LIB>

SYNTAX: int DoEndOfScope( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:	Handles end of scope "}" operation.

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoEndOfScope( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto int status;
   auto PHPJumpBackEntry *pJumpEntry;
   auto int bTakeLoop;

   if ( pServer->php_interpreter.iCurrentScope == 0 )
   {
      // Error we found a terminating code block when we were not
      // presently in a code block.
      return SC_SYNTAX_ERROR;
   }

   // Is there a jump back entry we need to take (possible loop scenario)?
   if ( pServer->php_interpreter.iTopJumpEntry >= 0 )
   {
      // Save the jump entry for quicker accessing.
      pJumpEntry = (&pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);

      // Check if the top jump entry is the current scope
      if ( pJumpEntry->cScope == pServer->php_interpreter.iCurrentScope )
      {
         if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_SCOPE ) )
         {
            // If the jump back type was for a loop then set the loop counter to remember we jumped back for a loop
            if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_LOOP ) )
               pServer->php_interpreter.cForLoopCounter = 1;   // Setting counter if jumpback occured

            // Check for a do while loop which requires more processing.
            if ( IS_SET( pJumpEntry->cJumpBackType, JBETYPE_DOWHILE ) )
            {
               // If its a do  {} while ( <exp> ); loop at this point we are past an ending code block, we need to only take the jump
               // if the statement inside the while ( <exp> ); is true.  If not we do not take the jump.
               if ( (status = CheckDOWHILELoop( pServer, pToken, &bTakeLoop )) != SC_OK )
                  return status;

               if ( bTakeLoop == TRUE )
               {
                  // Take the jump.
                  pServer->php_interpreter.ppFileLocation = pJumpEntry->ppJumpBackLocation;
                  MoveSlidingWindow( pServer, TRUE );

                  pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry since we took it.
               }
               else
               {
                  // Don't take the jump - but remove this entry from the jump back entry
                  pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry to remove it
               }
            }
            else
            {
               // Take the jump.
               pServer->php_interpreter.ppFileLocation = pJumpEntry->ppJumpBackLocation;
               MoveSlidingWindow( pServer, TRUE );

               pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry since we took it.
            }
         }
         else if ( IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_SWITCH ) )
         {
            // Don't take the jump - but remove this entry from the jump back entry
            pServer->php_interpreter.iTopJumpEntry--; // Decrement the jump back entry to remove it
         }
      }
   }

   // Free All variables in the current scope
   FreeVariablesInScope( pServer, pServer->php_interpreter.iCurrentScope );

   // Decrement the scope
   pServer->php_interpreter.iCurrentScope--;

   return SC_OK;
}




/* START FUNCTION DESCRIPTION ********************************************
SkipToEndOfScope                  <MAIN_PARSER.LIB>

SYNTAX: int SkipToEndOfScope( HttpServerStruct *pServer, int iScope )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:	Skips the file pointer to the end of scope.

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int SkipToEndOfScope( HttpServerStruct *pServer, int iScope )
{
   auto int iCurrentScope;
   auto TokenStruct oToken;

   iCurrentScope = pServer->php_interpreter.iCurrentScope;

   if ( iCurrentScope > iScope )
   	return SC_SYNTAX_ERROR;

   pServer->php_interpreter.bIgnoreHTMLCode = TRUE;

   // Scan ahead until we've skipped to the end of the current scope.
	while ( iCurrentScope >= iScope )
   {
   	if ( GetCurrentToken( &oToken, pServer ) == FALSE )
      	return SC_SYNTAX_ERROR;

      if ( oToken.ucTokenType == TYPE_SYMBOL )
      {
      	switch( oToken.TokenData.ucSymbolValue )
         {
         case TOKEN_CLOSE_SQUIGGLY:
      		iCurrentScope--;
            break;

         case TOKEN_OPEN_SQUIGGLY:
           	iCurrentScope++;
            break;
         }
      }
   }

   pServer->php_interpreter.bIgnoreHTMLCode = FALSE;

   // Reset the current scope after we've moved the file pointer past it.
   pServer->php_interpreter.iCurrentScope = iCurrentScope;

   return SC_OK;
}

/* START FUNCTION DESCRIPTION ********************************************
DoIFHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoIFHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoIFHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto int status;
   auto int iStatementType;
   auto char *ppStartCodeAddress;
   auto PHPJumpBackEntry *pJumpEntry;

   char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   status = 0;

   // We may end up looping in the case there is an else if block chained.
   for (;;)
   {
   	// Get the opening parenthesis (verify syntax).
	   if ( GetCurrentToken( pToken, pServer ) == FALSE )
	      return SC_END_OF_FILE_FOUND;

	   if ( pToken->ucTokenType != TYPE_SYMBOL ||
	        pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
	      return SC_SYNTAX_ERROR;

	   /* Call Expression Parser - tell it to end parsing when it reaches
	    * a closing paranthesis.
       * Note the DoExpressionParser function will eat the closing parenthesis
	    */
	   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_CLOSE_PAREN )) != SC_OK )
	      return status;

	   // Reuse status to see if we should execute the if statement or not...
	   status = ConvertVariableToBoolean( &oReturnValue );

	   if ( status == TRUE )
	   {
         // At this point we need to look ahead of the if statement to determine if there is a chained
      	// statement (such as an else if, or else statement) following it.  If we decide to take this
	      // the current if statement then we need to ignore the following chain of else if and the last else
	      // statement.  This requires arbitrary look ahead and skipping.
	      ppStartCodeAddress = pServer->php_interpreter.ppFileLocation;

         // Look ahead to see if this is a single statement or a code block.
         if ( GetCurrentToken( pToken, pServer ) == FALSE )
	            return SC_END_OF_FILE_FOUND;

         if ( pToken->ucTokenType == TYPE_SYMBOL &&
              pToken->TokenData.ucSymbolValue == TOKEN_OPEN_SQUIGGLY )
      	{
	         // Assume its a code block
	         iStatementType = 1;
	      }
	      else
	      {
	         // Assume its a single statement
	         iStatementType = 0;
	      }

         pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
         MoveSlidingWindow( pServer, TRUE );

	      // Skip past the block of code for any trailing else if statements or else statements.
         for (;;)
         {
         	if ( (status = SkipStatementOrCodeBlock( pServer, pToken )) != SC_OK )
	            return status;

	         if ( GetCurrentToken( pToken, pServer ) == FALSE )
            	break; // End of file, force the jump to this location.

	         if ( pToken->ucTokenType != TYPE_SYMBOL ||
	              pToken->TokenData.ucSymbolValue != TOKEN_ELSE )
	            break;
         }

         // Get the top entry on the jump back address stack.
         pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
         MoveSlidingWindow( pServer, TRUE );

      	pServer->php_interpreter.iTopJumpEntry++;
      	pJumpEntry = &(pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);


         // At this point pToken contains the location of where we need to jump to, to skip any remaining
         // portions of the if statement, add a JBETYPE_SCOPE or JBETYPE_STATEMENT onto the jump back stack
         // to skip the unexecuted part of the if statement.
         if ( iStatementType == 1 )
         {
            // Assume its a code block
         	pJumpEntry->ppJumpBackLocation = pToken->ppTokenLocation;
            pJumpEntry->cJumpBackType = 0;
	         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_SCOPE | JBETYPE_COND) );
	         pJumpEntry->cScope = (pServer->php_interpreter.iCurrentScope + 1);
	      }
	      else
	      {
	         // Assume its a single statement
	         pJumpEntry->ppJumpBackLocation = pToken->ppTokenLocation;
            pJumpEntry->cJumpBackType = 0;
	         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_STATEMENT | JBETYPE_COND) );
	      }

	      // We need to execute the if statement, pass execution back to the front end processor.
	      return SC_OK;
	   }
	   else
	   {
	      /*
	       *
	       * We need to skip the if statement, to do this we need to jump past the
	       * if statements code - please note we need to pass either a single
	       * statement or an entire code block.
	       *
	       */
	      if ( (status = SkipStatementOrCodeBlock( pServer, pToken )) != SC_OK )
	         return status; // We encountered an error skipping a single statement or code block.

         // Save the location to do single token look ahead.
         ppStartCodeAddress = pServer->php_interpreter.ppFileLocation;

         // Look ahead to see if this is a single statement or a code block.
         if ( GetCurrentToken( pToken, pServer ) == FALSE )
	            return SC_END_OF_FILE_FOUND;

         if ( pToken->ucTokenType != TYPE_SYMBOL ||
              pToken->TokenData.ucSymbolValue != TOKEN_ELSE )
         {
         	// End of the if statement, reset position before the single token look ahead and exit.
            pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
            MoveSlidingWindow( pServer, TRUE );

            return SC_OK;
         }

         // At this point we have passed over an else token, if we see an if token then we need to
         // rerun this loop, but if we see an else, we need to return execution to the front end processor
         // so that it will begin executing the else statement.
         // Look ahead to see if this is a single statement or a code block.

         // Save the location to do single token look ahead.
         ppStartCodeAddress = pServer->php_interpreter.ppFileLocation;

         if ( GetCurrentToken( pToken, pServer ) == FALSE )
	      {
         	// We reached the end of file, let the front end processor handle this reset our file location to the start
            // of the token.
            pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
            return SC_OK;
         }

         if ( pToken->ucTokenType != TYPE_SYMBOL ||
              pToken->TokenData.ucSymbolValue != TOKEN_IF )
         {
         	// We did not see an if token - assume we are in an else statement, return processing to the front end processor.
            // Make sure we reset our position before the single token look ahead.
            pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
            MoveSlidingWindow( pServer, TRUE );

            return SC_OK;
         }

         // At this point we will rerun the loop.
	   }
   }

   // Return execution to the front end processor
	return SC_OK;
}



/* START FUNCTION DESCRIPTION ********************************************
DoWHILEHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto PHPJumpBackEntry *pJumpEntry;
   auto char *ppJumpBackAddress;
   auto char *ppStartCodeAddress;
   auto int status;

	char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   /* resetting the for loop counter */
   pServer->php_interpreter.cForLoopCounter = 0;

   // Save the jump back address to the beginning of the while loop.
   ppJumpBackAddress = pToken->ppTokenLocation;

   status = 0;

   // Get the opening parenthesis (verify syntax).
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
      return SC_SYNTAX_ERROR;

   /* Call Expression Parser - tell it to end parsing when it reaches
    * a closing paranthesis.
    */
   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_CLOSE_PAREN )) != SC_OK )
      return status;

   // Reuse status to see if we should execute the if statement or not...
   status = ConvertVariableToBoolean( &oReturnValue );

   // Note the DoExpressionParser function will eat the closing parenthesis

   if ( status == TRUE )
   {
      // Execute what is inside the code block - this requires a
      // call to the PHPCodeHandler, inform it we wish to end processing.

      // We need to look ahead one token to determine if this is a statement or a code block
      // that is being executed next.
      ppStartCodeAddress = pServer->php_interpreter.ppFileLocation;

      if ( GetCurrentToken( pToken, pServer ) == FALSE )
         return SC_END_OF_FILE_FOUND;

      pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
      MoveSlidingWindow( pServer, TRUE );

      pServer->php_interpreter.iTopJumpEntry++;
      pJumpEntry = &(pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);

      if ( pToken->ucTokenType == TYPE_SYMBOL &&
           pToken->TokenData.ucSymbolValue == TOKEN_OPEN_SQUIGGLY )
      {
         // Assume its a code block
         pJumpEntry->ppJumpBackLocation = ppJumpBackAddress;
         pJumpEntry->cJumpBackType = 0;
         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_SCOPE | JBETYPE_LOOP) );
         pJumpEntry->cScope = (pServer->php_interpreter.iCurrentScope + 1);
      }
      else
      {
         // Assume its a single statement
         pJumpEntry->ppJumpBackLocation = ppJumpBackAddress;
         pJumpEntry->cJumpBackType = 0;
         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_STATEMENT | JBETYPE_LOOP) );
      }
   }
   else
   {
      /*
      *
      * We need to skip the while statement, to do this we need to jump past the
      * if statements code - please note we need to pass either a single
      * statement or an entire code block.
      *
      */
      if ( (status = SkipStatementOrCodeBlock( pServer, pToken )) != SC_OK )
         return status; // We encountered an error skipping a single statement or code block.

      return SC_OK;
   }



   // Return execution to the front end processor
	return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoDOWHILEHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoDOWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoDOWHILEHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPJumpBackEntry *pJumpEntry;
   auto char *ppJumpBackAddress;

   // Save the jump back address to the beginning of the token
   ppJumpBackAddress = pToken->ppTokenLocation;

   /* reset the for loop counter */
   pServer->php_interpreter.cForLoopCounter = 0;

   // All do while loops must be inside a code block.
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_OPEN_SQUIGGLY )
      return SC_SYNTAX_ERROR;


   // Reset the file pointer to point to the beginning of the code block
   pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
   MoveSlidingWindow( pServer, TRUE );

   pServer->php_interpreter.iTopJumpEntry++;
   pJumpEntry = &(pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);


   // Assume its a code block
   pJumpEntry->ppJumpBackLocation = ppJumpBackAddress;
   pJumpEntry->cJumpBackType = 0;
   SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_SCOPE | JBETYPE_LOOP | JBETYPE_DOWHILE) );
   pJumpEntry->cScope = (pServer->php_interpreter.iCurrentScope + 1);

   // Return execution to the front end processor
	return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
CheckDOWHILELoop                  <MAIN_PARSER.LIB>

SYNTAX: int CheckDOWHILELoop( HttpServerStruct *pServer, TokenStruct *pToken, char *bTakeLoop )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int CheckDOWHILELoop( HttpServerStruct *pServer, TokenStruct *pToken, int *bTakeLoop )
{
	auto int status;
	auto PHPVariable oReturnValue;

   char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   // Get the opening parenthesis (verify syntax).
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_WHILE )
      return SC_SYNTAX_ERROR;

   // Get the opening parenthesis (verify syntax).
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
      return SC_SYNTAX_ERROR;

   /* Call Expression Parser - tell it to end parsing when it reaches
    * a closing paranthesis.
    */
   if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_CLOSE_PAREN )) != SC_OK )
      return status;

   // Reuse status to see if we should execute the if statement or not...
   (*bTakeLoop) = ConvertVariableToBoolean( &oReturnValue );

   // Get the semicolon to verify syntax
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_SEMICOLON )
      return SC_SYNTAX_ERROR;

   // Return execution to the front end processor
	return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
DoFORHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoFORHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoFORHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oReturnValue;
   auto PHPJumpBackEntry *pJumpEntry;
   auto char *ppJumpBackAddress;
   auto char *ppStartCodeAddress;
   auto int status;
   auto int cForBool;
   HttpServerStruct hServerSave;

   char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oReturnValue.iVNum = VAR_VNUM_TEMPORARY;
   oReturnValue.VarData.ppStringValue = szReturnTemporary;

   // Save the jump back address to the beginning of the while loop.
   ppJumpBackAddress = pToken->ppTokenLocation;

   status = 0;

   // Get the opening parenthesis (verify syntax).
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
        pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
      return SC_SYNTAX_ERROR;

   /* Call Expression Parser - tell it to end parsing when it reaches
    * a semicolon.
    */
   if( !pServer->php_interpreter.cForLoopCounter )
   {
	   // Perform look ahead.
	   if ( GetCurrentToken( pToken, pServer ) == FALSE )
	      return SC_END_OF_FILE_FOUND;
	   /* Check for next token equal to semicolon */
	   if ( pToken->ucTokenType == TYPE_SYMBOL &&
	        pToken->TokenData.ucSymbolValue == TOKEN_SEMICOLON )
	   {
	         // Token is a semicolon, Do nothing
	   }
	   else
	   {
	         // Token is not a semicolon, jump gack to previous token
	         pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
            MoveSlidingWindow( pServer, TRUE );

	         if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_SEMICOLON )) != SC_OK )
	            return status;
	   }
   }
   else
   {
      /* Skipping the execution of the first param. Frist param is executed
      once in the for loop */
      SkipToFirstOccurSymbol( pServer, pToken, TOKEN_SEMICOLON );
   }
   // Perform look ahead on second param of for loop
   if ( GetCurrentToken( pToken, pServer ) == FALSE )
      return SC_END_OF_FILE_FOUND;
   /* Check for next token equal to semicolon */
   if ( pToken->ucTokenType == TYPE_SYMBOL &&
        pToken->TokenData.ucSymbolValue == TOKEN_SEMICOLON )
   {
     		// Token is a semicolon, for loop is automatically true
         cForBool = TRUE;
   }
   else
   {
       	// Token is not a semicolon, jump gack to previous token
         pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
         MoveSlidingWindow( pServer, TRUE );

   		if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_SEMICOLON )) != SC_OK )
      		return status;

   		/* cForBool indicates whether to execute loop */
   		cForBool = ConvertVariableToBoolean( &oReturnValue );
   }

	// Perform look ahead on third param of for loop
   if( pServer->php_interpreter.cForLoopCounter )
   {
   	if ( GetCurrentToken( pToken, pServer ) == FALSE )
      	return SC_END_OF_FILE_FOUND;
   	/* Check for next token equal to semicolon */
   	if ( pToken->ucTokenType == TYPE_SYMBOL &&
        	pToken->TokenData.ucSymbolValue == TOKEN_SEMICOLON )
   	{
     		// Token is a semicolon, Do nothing
   	}
   	else
   	{
       	// Token is not a semicolon, jump back to previous token
         pServer->php_interpreter.ppFileLocation = pToken->ppTokenLocation;
         MoveSlidingWindow( pServer, TRUE );

   		if ( (status = DoExpressionParser( pServer, &oReturnValue, TOKEN_CLOSE_PAREN )) != SC_OK )
      		return status;
   	}
      pServer->php_interpreter.cForLoopCounter = 0;

   }
   else
   {
      /* Skipping the execution of the third param. Third param is executed at the end
      of the for loop */
      SkipToFirstOccurSymbol( pServer, pToken, TOKEN_CLOSE_PAREN );
   }

   // Note the DoExpressionParser function will eat the closing parenthesis
   if ( cForBool == TRUE )
   {
      // Execute what is inside the code block - this requires a
      // call to the PHPCodeHandler, inform it we wish to end processing.

      // We need to look ahead one token to determine if this is a statement or a code block
      // that is being executed next.
      ppStartCodeAddress = pServer->php_interpreter.ppFileLocation;

      if ( GetCurrentToken( pToken, pServer ) == FALSE )
         return SC_END_OF_FILE_FOUND;

      pServer->php_interpreter.ppFileLocation = ppStartCodeAddress;
      MoveSlidingWindow( pServer, TRUE );

      pServer->php_interpreter.iTopJumpEntry++;
      pJumpEntry = &(pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);

      if ( pToken->ucTokenType == TYPE_SYMBOL &&
           pToken->TokenData.ucSymbolValue == TOKEN_OPEN_SQUIGGLY )
      {
         // Assume its a code block
         pJumpEntry->ppJumpBackLocation = ppJumpBackAddress;
         pJumpEntry->cJumpBackType = 0;
         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_SCOPE | JBETYPE_LOOP) );
         pJumpEntry->cScope = (pServer->php_interpreter.iCurrentScope + 1);
      }
      else
      {
         // Assume its a single statement
         pJumpEntry->ppJumpBackLocation = ppJumpBackAddress;
         pJumpEntry->cJumpBackType = 0;
         SET_BIT( pJumpEntry->cJumpBackType, (JBETYPE_STATEMENT | JBETYPE_LOOP) );
      }
   }
   else
   {
      /*
      *
      * We need to skip the while statement, to do this we need to jump past the
      * if statements code - please note we need to pass either a single
      * statement or an entire code block.
      *
      */
      if ( (status = SkipStatementOrCodeBlock( pServer, pToken )) != SC_OK )
         return status; // We encountered an error skipping a single statement or code block.

      return SC_OK;
   }



   // Return execution to the front end processor
	return SC_OK;
}

/* START FUNCTION DESCRIPTION ********************************************
DoSWITCHHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoSWITCHHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoSWITCHHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto PHPVariable oSwitchValue;
   auto PHPVariable oCaseValue;
   auto int status;
   auto int lSwitchValue;
   auto int lCaseValue;
   auto PHPJumpBackEntry *pJumpEntry;

	char szReturnTemporary1[PHP_MAX_STRINGLENGTH+1];
   oSwitchValue.iVNum = VAR_VNUM_TEMPORARY;
   oSwitchValue.VarData.ppStringValue = szReturnTemporary1;

	char szReturnTemporary2[PHP_MAX_STRINGLENGTH+1];
   oCaseValue.iVNum = VAR_VNUM_TEMPORARY;
   oCaseValue.VarData.ppStringValue = szReturnTemporary2;

   status = 0;
	// Get the opening parenthesis (verify syntax).
	if ( GetCurrentToken( pToken, pServer ) == FALSE )
   	return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
   	  pToken->TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
   	return SC_SYNTAX_ERROR;

   /* Call Expression Parser - tell it to end parsing when it reaches
    * a closing paranthesis.
    */
   if ( (status = DoExpressionParser( pServer, &oSwitchValue, TOKEN_CLOSE_PAREN )) != SC_OK )
   	return status;

   // Get the opening squiggly (verify syntax).
	if ( GetCurrentToken( pToken, pServer ) == FALSE )
   	return SC_END_OF_FILE_FOUND;

   if ( pToken->ucTokenType != TYPE_SYMBOL ||
   	  pToken->TokenData.ucSymbolValue != TOKEN_OPEN_SQUIGGLY )
   	return SC_SYNTAX_ERROR;

    pServer->php_interpreter.iCurrentScope++; // Incrementing the scope

    pServer->php_interpreter.iTopJumpEntry++;
    pJumpEntry = &(pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry]);


    // Setup a jump back entry for a switch statement - even tho it will never be taken.
    pJumpEntry->ppJumpBackLocation = pToken->ppTokenLocation;
    pJumpEntry->cJumpBackType = 0;
    SET_BIT( pJumpEntry->cJumpBackType, JBETYPE_SWITCH );
    pJumpEntry->cScope = pServer->php_interpreter.iCurrentScope;


   // Jump to the first case statement
   while (status = SkipCaseStatement( pServer, pToken ) == SC_OK )
   {
   	/* Check the previous token for default symbol.  Always excute default
      	symbol when it is encountered */

      if ( pToken->TokenData.ucSymbolValue == TOKEN_DEFAULT )
      {
   		// Get the colon symbol (verify syntax).
			if ( GetCurrentToken( pToken, pServer ) == FALSE )
   			return SC_END_OF_FILE_FOUND;

   		if ( pToken->ucTokenType != TYPE_SYMBOL ||
   	  		pToken->TokenData.ucSymbolValue != TOKEN_COLON )
            return SC_SYNTAX_ERROR;

			return SC_OK;
      }

   	/* Call Expression Parser - tell it to end parsing when it reaches
    	* a colon.
    	*/
   	if ( (status = DoExpressionParser( pServer, &oCaseValue, TOKEN_COLON )) != SC_OK )
   		return status;

   	// Check if switch value is equal with case value
		if ( DoNormalComparison( pServer, &oSwitchValue, &oCaseValue ) == TRUE )
		{
   		return SC_OK;
   	}
   }
   if ( status == SC_ENDOFSCOPE )
   {
   	pServer->php_interpreter.iCurrentScope --; // Incrementing the scope
   	return SC_OK;
   }
   else
   	return status; // We encountered an error
}


/* START FUNCTION DESCRIPTION ********************************************
SkipCaseStatement                  <MAIN_PARSER.LIB>

SYNTAX: int SkipToFirstOccurSymbol( HttpServerStruct *pServer, TokenStruct * pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION: Search to the file pointer for a given symbol and jump to that location.
             The symbol jumping to is also within the same scope as the start of
             the search.
PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  SC_OK case statement found
					SC_SYNTAX_ERROR case statement not found

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int SkipToFirstOccurSymbol( HttpServerStruct *pServer, TokenStruct *pToken, unsigned int ucSymbol )
{
	auto int iCurrentScope;

   iCurrentScope = 0;
   pServer->php_interpreter.bIgnoreHTMLCode = TRUE;

   while ( GetCurrentToken( pToken, pServer ) == TRUE )
   {
   	if ( pToken->ucTokenType ==  TYPE_SYMBOL )
      {
   		switch ( pToken->TokenData.ucSymbolValue )
      	{
       	case TOKEN_OPEN_SQUIGGLY: // Opening code block - record that we have seen this.
            iCurrentScope++;
            break;

         case TOKEN_CLOSE_SQUIGGLY: // Close code block
            iCurrentScope--;
            if ( iCurrentScope < 0 ) // We don't have an open code block yet - but we have received a close code block - this is a syntax error.
            	return SC_ENDOFSCOPE;

            break;

      	}

         if( pToken->TokenData.ucSymbolValue == ucSymbol && iCurrentScope == 0 )
         {
         	pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
            return SC_OK;
      	}
   	}
   }
   return SC_SYNTAX_ERROR;
}

/* START FUNCTION DESCRIPTION ********************************************
SkipCaseStatement                  <MAIN_PARSER.LIB>

SYNTAX: int SkipCaseStatement( HttpServerStruct *pServer, TokenStruct * pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION: Skip to the next case statement

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  SC_OK case statement found
					SC_SYNTAX_ERROR case statement not found

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int SkipCaseStatement( HttpServerStruct *pServer, TokenStruct *pToken )
{
	auto int iCurrentScope;

   iCurrentScope = 0;
   pServer->php_interpreter.bIgnoreHTMLCode = TRUE;

   while ( GetCurrentToken( pToken, pServer ) == TRUE )
   {
   	if ( pToken->ucTokenType ==  TYPE_SYMBOL )
      {
   		switch ( pToken->TokenData.ucSymbolValue )
      	{
       	case TOKEN_OPEN_SQUIGGLY: // Opening code block - record that we have seen this.
            iCurrentScope++;
            break;

         case TOKEN_CLOSE_SQUIGGLY: // Close code block
            iCurrentScope--;

            if ( iCurrentScope < 0 ) // We don't have an open code block yet - but we have received a close code block - this is a syntax error.
            	return SC_ENDOFSCOPE;

            break;
         case TOKEN_CASE:
         	if ( iCurrentScope == 0 )
            {
            	pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
            	return SC_OK;
            }
            break;

         case TOKEN_DEFAULT:
         	if ( iCurrentScope == 0 )
            {
            	pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
            	return SC_OK;
            }
            break;
      	}
      }
   }
   return SC_SYNTAX_ERROR;
}

/* START FUNCTION DESCRIPTION ********************************************
DoCaseHandler                  <MAIN_PARSER.LIB>

SYNTAX: int DoCaseHandler( HttpServerStruct *pServer, TokenStruct *pToken )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns 0 if there were no errors while processing, and a
					status code otherwise.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoCaseHandler( HttpServerStruct *pServer, TokenStruct *pToken )
{
	auto int status;

   // Make sure this jump entry is a switch statement.
   if ( pServer->php_interpreter.iTopJumpEntry < 0 ||
   	  !IS_SET( pServer->php_interpreter.oJumpBackTable[pServer->php_interpreter.iTopJumpEntry].cJumpBackType, JBETYPE_SWITCH ) )
   {
   	// A case statement sitting outside on its own not in a switch statement???
   	return SC_SYNTAX_ERROR;
   }

   if ( (status = SkipToFirstOccurSymbol( pServer, pToken, TOKEN_COLON )) != SC_OK )
   	return status;

   return SC_OK;
}

/* START FUNCTION DESCRIPTION ********************************************
SkipStatementOrCodeBlock                  <MAIN_PARSER.LIB>

SYNTAX: int SkipStatementOrCodeBlock( HttpServerStruct *pServer )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Skip a single statement or skip a code block - making sure to
					handle ?> <?php blocks and nested code blocks.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int SkipStatementOrCodeBlock( HttpServerStruct *pServer, TokenStruct *pToken )
{
   auto int iCurrentScope;
   auto int cIsCodeBlock;

   iCurrentScope = 0;
   cIsCodeBlock = FALSE;
   pServer->php_interpreter.bIgnoreHTMLCode = TRUE;

   while ( GetCurrentToken( pToken, pServer ) == TRUE )
   {
   	if ( pToken->ucTokenType ==  TYPE_SYMBOL )
      {
   		switch ( pToken->TokenData.ucSymbolValue )
      	{
       	case TOKEN_OPEN_SQUIGGLY: // Opening code block - record that we have seen this.
         	cIsCodeBlock = TRUE;
            iCurrentScope++;
            break;

         case TOKEN_CLOSE_SQUIGGLY: // Close code block
            iCurrentScope--;

            if ( iCurrentScope == 0 )
            {
            	pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
            	return SC_OK;
            }
            break;

         case TOKEN_SEMICOLON:
            if ( cIsCodeBlock == FALSE )
            {
            	pServer->php_interpreter.bIgnoreHTMLCode = FALSE;
	            return SC_OK;
            }
            break;
      	}
      }
   }

   return SC_SYNTAX_ERROR;
}

/* START FUNCTION DESCRIPTION ********************************************
DoExpressionParser                  <MAIN_PARSER.LIB>

SYNTAX: int DoExpressionParser( HttpServerStruct *pServer, PHPVariable *pReturn, unsigned int ucEndSymbol )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.
PARAMETER2: Pointer to the variable that will hold the return data of the expression evaluation.
PARAMETER3: The symbol that denotes the end of the expression (usually semicolon, comma, or end parenthesis)

RETURN VALUE:  Performs an expression evaluation on the current expression using
					a shift/reduce expression modified slightly to be more efficient
               and implement error checking (see Douglas Paul Gregor - Sugar
               Parser).

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoExpressionParser( HttpServerStruct *pServer, PHPVariable *pReturn, unsigned int ucEndSymbol )
{
   auto TokenStruct T1;
   auto int cExpState;
   auto int cParenCount;
   auto int cOpType;
   auto int cAction;
   auto int bDone;
   auto int cState;

   auto char *ppFuncNameLoc;
   auto int iFuncNameLen;
   auto char *ppArgLoc;

   auto int cOperatorStackTop;
   auto int cOperandStackTop;

   auto int iVar;
   auto int iTokenCount;

   auto int iAllocatedFunctionArguments[EXP_FUNCARG_SIZE];
   auto PHPOperator oOperatorStack[EXP_OPERATOR_STACK_SIZE];

   // Contains the list of variables allocated for the operand stack.
   auto int iAllocatedOperandStack[EXP_OPERAND_STACK_SIZE];

   auto PHPVariable *pOperandStack[EXP_OPERAND_STACK_SIZE];

   // Pre allocate the operand stack.
   for ( iVar = 0; iVar < EXP_OPERAND_STACK_SIZE; iVar++ )
   {
   	if ( (iAllocatedOperandStack[iVar] = CreateVariable( pServer )) == -1 )
      	return SC_OUTOFMEMORY;
   }

   // Pre allocate the function argument variables
   for ( iVar = 0; iVar < EXP_FUNCARG_SIZE; iVar++ )
   {
      if ( (iAllocatedFunctionArguments[iVar] = CreateVariable( pServer )) == -1 )
      	return SC_OUTOFMEMORY;
   }

   //
   // Note to self: How to make variables work properly.
   // Make the operand stack a stack of pointers and then
   // whenever a pointer refers to a token have it point to a temp
   // list of PHPVariables.  If it refers to a variable,
   // have it point to the variable data structure - its that
   // simple.  (We might run into problems if/when we choose
   // to make variables stored in xmem) :P
   //

   cExpState = EXP_STATE_PRE;
   cParenCount = 0;
   cOperatorStackTop = 0;
   cOperandStackTop = 0;

   // Incase we get called with a blank expression - then we simply copy the first variable allocated - which is always
   // initialized to int 0.
   pOperandStack[0] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[0]]);

   while ( GetCurrentToken( &T1, pServer ) == TRUE )
   {
   	// Check the type of the token
   	switch ( T1.ucTokenType )
      {
      case TYPE_SYMBOL:	// Is it a symbol (most likely an operator).
      	// Check to see if this is the terminating symbol
         if ( T1.TokenData.ucSymbolValue == ucEndSymbol
              && cParenCount == 0 )
         {
#ifdef MPARSER_VERBOSE
				printf( "MPARSER: End statement (reducing)\n" );
#endif

         	// End of processing for expression - for reduction
            while ( cOperatorStackTop > 0 )
            {
               if ( (cState = DoExpressionReduce( pServer, oOperatorStack, pOperandStack, &cOperatorStackTop, &cOperandStackTop, iAllocatedOperandStack )) != SC_OK )
                 	return cState;
            }

#ifdef MPARSER_VERBOSE
				printf( "MPARSER: Copying return (after reducing)\n" );
#endif

            CopyPHPVariable( pServer, pReturn, pOperandStack[0] );

#ifdef MPARSER_VERBOSE
				printf( "MPARSER: Freeing variables (after reducing)\n" );
#endif

            // Free the allocated list of variables
   	      for ( iVar = 0; iVar < EXP_OPERAND_STACK_SIZE; iVar++ )
	         {
            	if ( FreeVariable( pServer, iAllocatedOperandStack[iVar] ) == FALSE )
               	return SC_INTERNALERROR;
	         }

            // Free the allocated list of function arguments
   	      for ( iVar = 0; iVar < EXP_FUNCARG_SIZE; iVar++ )
	         {
            	if ( FreeVariable( pServer, iAllocatedFunctionArguments[iVar] ) == FALSE )
               	return SC_INTERNALERROR;
	         }

#ifdef MPARSER_VERBOSE
				printf( "MPARSER: Done Freeing variables (after reducing)\n" );
#endif

            return SC_OK;
         }

         // Handle symbols in the expression stream.
      	switch ( T1.TokenData.ucSymbolValue )
         {
         case TOKEN_OPEN_PAREN:
         	cParenCount++;
            cOpType = EXP_OPTYPE_CONFIX_OPEN;
            break;

         case TOKEN_CLOSE_PAREN:
         	cParenCount--;
            cOpType = EXP_OPTYPE_CONFIX_CLOSE;
            break;

         default:
         	cOpType = GetOperatorType( T1.TokenData.ucSymbolValue, cExpState );
         	break;
         }

         // Check to see if the operator was not found
         if ( cOpType == EXP_OPTYPE_NOTFOUND )
         	return SC_SYNTAX_ERROR;	// Operator not found - return syntax error


         /*
          * Verify the correctness of the expression thus far using the state
          * machine proposed by Douglas Paul Gregor
          */
         if ( cExpState == EXP_STATE_PRE )
         {
         	switch( cOpType )
         	{
         	case EXP_OPTYPE_PREFIX:			// -2
            case EXP_OPTYPE_CONFIX_OPEN:	// (
            	break; // No change

            case EXP_OPTYPE_INFIX:			// 3 + 2
            case EXP_OPTYPE_POSTFIX:		// 3!
            case EXP_OPTYPE_CONFIX_CLOSE:	// )
            	return SC_SYNTAX_ERROR; // Error - can't have an infix, postfix, or confix close operator before an operand.
            	break;

            default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - Op Type not found in DoExpressionParser()\n" );
#endif
               break;
         	}
         }
         else //  cExpState == EXP_STATE_POST
         {
         	switch( cOpType )
            {
            case EXP_OPTYPE_INFIX: 			// 3 + 2
            	cExpState = EXP_STATE_PRE;
               break;

            case EXP_OPTYPE_POSTFIX: 		// 3!
            case EXP_OPTYPE_CONFIX_CLOSE: // )
            	break; // No change

            case EXP_OPTYPE_PREFIX:			// -3
            case EXP_OPTYPE_CONFIX_OPEN:	// (
            	return SC_SYNTAX_ERROR; // Error can't have a prefix, confix open operator after an operand.
               break;

            default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - Op Type not found in DoExpressionParser()\n" );
#endif
               break;
            }
			} // Done verifying correctness

         /*
          * At this point the expression is correct and the operator has been resolved
          * to its respective type, now either a shift/reduce needs to be performed
          * on the two stacks depending on the current token (operator) and the operator
          * located on the top of the stack.
          */
         do
         {
         	if ( cOperatorStackTop == 0 )
         	{
         		// No operators on the stack - force a shift
            	cAction = EXP_ACTION_SHIFT;
         	}
         	else
         	{
         		// Lookup the action from the expression parser table.
         		cAction = ExpressionParserTable[ (oOperatorStack[(cOperatorStackTop-1)].cOperatorType) ][cOpType];
         	}


	         /*
	          * Now perform the action - either shift/reduce/compare precedence/or associativity.
	          */
	         switch (cAction)
	         {
	         case EXP_ACTION_SHIFT: // Shift operation - push onto operator stack.
	            if ( cOperatorStackTop >= EXP_OPERATOR_STACK_SIZE )
	            {
#ifdef MPARSER_VERBOSE
	               printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operator stack in DoExpressionParser()\n" );
#endif
	               return SC_SYNTAX_ERROR;
	            }

	            // Shift onto the top of the operator stack
	            oOperatorStack[cOperatorStackTop].cOperatorType = cOpType;
	            oOperatorStack[cOperatorStackTop].ucOperatorSymbol = T1.TokenData.ucSymbolValue;

	            // Increment the top of the operator stack by 1
	            cOperatorStackTop++;

               // We are done since we have shifted this operator onto the operator stack.
	            bDone = TRUE;
	            break;

	         case EXP_ACTION_REDUCE:
            	// Call the reduce handler function for doing the reduction of the operator/operand stack
	            if ( (cState = DoExpressionReduce( pServer, oOperatorStack, pOperandStack, &cOperatorStackTop, &cOperandStackTop, iAllocatedOperandStack )) != SC_OK )
                 	return cState;

               // We are not done since we have not shifted the current operator onto the operator stack yet.
               bDone = FALSE;
               break;

	         case EXP_ACTION_PRECEDENCE:
	            // Compare precedence, do shift or reduce.
               if ( GetOperatorPrecedence( T1.TokenData.ucSymbolValue ) < GetOperatorPrecedence( oOperatorStack[(cOperatorStackTop-1)].ucOperatorSymbol ) )
               {
               	// Shift...
                  if ( cOperatorStackTop >= EXP_OPERATOR_STACK_SIZE )
	               {
#ifdef MPARSER_VERBOSE
	                  printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operator stack in DoExpressionParser()\n" );
#endif
	                  return SC_SYNTAX_ERROR;
	               }

	               // Shift onto the top of the operator stack
	               oOperatorStack[cOperatorStackTop].cOperatorType = cOpType;
	               oOperatorStack[cOperatorStackTop].ucOperatorSymbol = T1.TokenData.ucSymbolValue;

	               // Increment the top of the operator stack by 1
	               cOperatorStackTop++;

                  // Not done since we shifted.
                  bDone = TRUE;
               }
               else
               {
               	// Reduce...
                  // Call the reduce handler function for doing the reduction of the operator/operand stack
	            	if ( (cState = DoExpressionReduce( pServer, oOperatorStack, pOperandStack, &cOperatorStackTop, &cOperandStackTop, iAllocatedOperandStack )) != SC_OK )
                 		return cState;

               	bDone = FALSE;
               }
	            break;

	         case EXP_ACTION_PRECEDENCE_ASSOC:
	            // Compare precedence, do shift or reduce.
               cState = GetOperatorPrecedence( oOperatorStack[(cOperatorStackTop-1)].ucOperatorSymbol ) - GetOperatorPrecedence( T1.TokenData.ucSymbolValue );
               if ( cState < 0  )
               {
               	// Shift...
                  if ( cOperatorStackTop >= EXP_OPERATOR_STACK_SIZE )
	               {
#ifdef MPARSER_VERBOSE
	                  printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operator stack in DoExpressionParser()\n" );
#endif
	                  return SC_SYNTAX_ERROR;
	               }

	               // Shift onto the top of the operator stack
	               oOperatorStack[cOperatorStackTop].cOperatorType = cOpType;
	               oOperatorStack[cOperatorStackTop].ucOperatorSymbol = T1.TokenData.ucSymbolValue;

	               // Increment the top of the operator stack by 1
	               cOperatorStackTop++;

                  // Not done since we shifted.
                  bDone = TRUE;
               }
               else if ( cState > 0 )
               {
               	// Reduce...
                  // Call the reduce handler function for doing the reduction of the operator/operand stack
	            	if ( (cState = DoExpressionReduce( pServer, oOperatorStack, pOperandStack, &cOperatorStackTop, &cOperandStackTop, iAllocatedOperandStack )) != SC_OK )
                 		return cState;

               	bDone = FALSE;
               }
               else
               {
               	// They are equal - compare associativity.
                  cState = GetOperatorAssociativity( T1.TokenData.ucSymbolValue );

                  if ( cState == EXP_ASSOC_LEFT || cState == EXP_ASSOC_NONE )
                  {
                  	// Reduce
                     // Call the reduce handler function for doing the reduction of the operator/operand stack
	            		if ( (cState = DoExpressionReduce( pServer, oOperatorStack, pOperandStack, &cOperatorStackTop, &cOperandStackTop, iAllocatedOperandStack )) != SC_OK )
                 			return cState;

               		bDone = FALSE;
                  }
                  else
                  {
                     // Shift...
                  	if ( cOperatorStackTop >= EXP_OPERATOR_STACK_SIZE )
	                  {
	#ifdef MPARSER_VERBOSE
	                     printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operator stack in DoExpressionParser()\n" );
	#endif
	                     return SC_SYNTAX_ERROR;
	                  }

	                  // Shift onto the top of the operator stack
	                  oOperatorStack[cOperatorStackTop].cOperatorType = cOpType;
	                  oOperatorStack[cOperatorStackTop].ucOperatorSymbol = T1.TokenData.ucSymbolValue;

	                  // Increment the top of the operator stack by 1
	                  cOperatorStackTop++;

	                  // Not done since we shifted.
	                  bDone = TRUE;
                  }
               }
	            break;

	         default:
	            // Syntax error.
	            return SC_SYNTAX_ERROR;
	            break;
	         }
			} while ( bDone == FALSE );

         break; // END TYPE_SYMBOL

      case TYPE_VARIABLE:	// Variable is an operand - push onto operand stack
         if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
	      {
#ifdef MPARSER_VERBOSE
	      	printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
				return SC_SYNTAX_ERROR;
			}

         // Check the correctness of the expression - we should not get an operand in the post operand state.
         if ( cExpState == EXP_STATE_POST )
         	return SC_SYNTAX_ERROR;

         // Save the location of the variable name
         ppArgLoc = T1.ppTokenLocation;

         // Save the length of the variable name
         iTokenCount = T1.TokenData.ucIdentifierLen;

         if ( iTokenCount > PHP_MAX_VARNAME_SIZE )
         {
         	return SC_SYNTAX_ERROR; // Variable name too big.
         }

         // Check for a special case - ie a special variable like _POST, _GET, etc
         if ( GetCurrentToken( &T1, pServer ) == TRUE &&
         	  T1.ucTokenType == TYPE_SYMBOL &&
              T1.TokenData.ucSymbolValue == TOKEN_OPEN_BRACKET )
	      {
         	// Its a case of a special variable - call the expression parser to get the return value for
            // the arguments passed between the [ ].

            // We treat this like a function call with a single argument.
            if ( (cState = DoExpressionParser( pServer, &(pServer->php_interpreter.oVariableTable[iAllocatedFunctionArguments[0]]), TOKEN_CLOSE_BRACKET )) != SC_OK )
	            return cState;

            // Check for overload.
            if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
         	{
#ifdef MPARSER_VERBOSE
	            printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
	            return SC_SYNTAX_ERROR;
	         }

	         // Shift onto the operand stack a temporary value - since this is a constant
	         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[cOperandStackTop]]);

            // Load into root memory the variable name into the string buffer
            xmem2root( g_oStringBuffer, (ppArgLoc+1), iTokenCount );

	         // Call the special function to handle this "array" like behavior
	         if ( (cState = CallSpecialFunction( pServer, pOperandStack[cOperandStackTop], iAllocatedFunctionArguments[0], g_oStringBuffer, iTokenCount )) != SC_OK )
	            return cState;

	         // Increment the top of the operand stack by 1
	         cOperandStackTop++;
         }
         else
         {
            // Backup before the [ that was read ahead.
            pServer->php_interpreter.ppFileLocation = T1.ppTokenLocation;
            MoveSlidingWindow( pServer, TRUE );

            // Load into root memory the variable name into the string buffer
            xmem2root( g_oStringBuffer, (ppArgLoc+1), iTokenCount );

         	// Find or create a variable for this identifier
	         if ( (iVar = FindOrCreateVariable( pServer, g_oStringBuffer, iTokenCount )) == -1 )
	            return SC_OUTOFMEMORY;

	         // Link the element on the operand stack to the variable table since this is a variable
	         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iVar]);

	         // Increment the top of the operand stack by 1
	         cOperandStackTop++;
         }

         // Change the expression state to post operand state.
         cExpState = EXP_STATE_POST;

      	break;

      case TYPE_IDENTIFIER:  // Identifier is most likely a function call.
         ppFuncNameLoc = T1.ppTokenLocation;
         iFuncNameLen = T1.TokenData.ucIdentifierLen;

         if ( GetCurrentToken( &T1, pServer ) == FALSE )
	      	return SC_END_OF_FILE_FOUND;

         if ( T1.ucTokenType != TYPE_SYMBOL ||
              T1.TokenData.ucSymbolValue != TOKEN_OPEN_PAREN )
         	return SC_SYNTAX_ERROR;

         ppArgLoc = pServer->php_interpreter.ppFileLocation;
         cState = 0;  	// Indicates number of parenthesis.
         iVar = 0;      // Indicates number of arguments.
         iTokenCount = 0; // Indicates the number of tokens for an argument

         do
         {
            if ( GetCurrentToken( &T1, pServer ) == FALSE )
	      		return SC_END_OF_FILE_FOUND;

            iTokenCount++;

         	if ( T1.ucTokenType == TYPE_SYMBOL )
            {
            	switch( T1.TokenData.ucSymbolValue )
               {
               	case TOKEN_OPEN_PAREN:
                  	cState++;
                     break;

                  case TOKEN_CLOSE_PAREN:
                  	cState--;
                     break;

                  case TOKEN_COMMA:
                  	if ( iTokenCount <= 1 )
                     {
                     	// Error we found a comma, but the number of tokens was 1 (meaning we only see a comma - therefore there is no argument.
                        return SC_SYNTAX_ERROR;
                     }

                     // At this point we have a comma, so we need to parse this string of tokens,
                     // up to the comma and interpret them and store the result in the call list
                     // to be passed to the function.
                     pServer->php_interpreter.ppFileLocation = ppArgLoc;
                     MoveSlidingWindow( pServer, TRUE );

                     if ( (cState = DoExpressionParser( pServer, &(pServer->php_interpreter.oVariableTable[iAllocatedFunctionArguments[iVar]]), TOKEN_COMMA )) != SC_OK )
                     	return cState;

                     ppArgLoc = pServer->php_interpreter.ppFileLocation;

                     // Move to the next argument
                     iVar++;

                     // Reset the number of parenthesis read.
                     cState = 0;

                     // Reset the token count
                     iTokenCount = 0;

                     break;
               }
            }
         } while ( cState >= 0 );

         // Do we have arguments, but no tokens after the comma? ie func( 1, ); - this  is a syntax error.
         if ( iVar > 0 && iTokenCount <= 1 )
         	return SC_SYNTAX_ERROR;

         if ( iTokenCount > 1 )
         {
         	// At this point we have a closing parenthesis, so we need to parse this string of tokens,
	         // up to the ) and interpret them and store the result in the call list
	         // to be passed to the function.
	         pServer->php_interpreter.ppFileLocation = ppArgLoc;
            MoveSlidingWindow( pServer, TRUE );

	         if ( (cState = DoExpressionParser( pServer, &(pServer->php_interpreter.oVariableTable[iAllocatedFunctionArguments[iVar]]), TOKEN_CLOSE_PAREN )) != SC_OK )
	            return cState;

	         // Move to the next argument
	         iVar++;
         }
         // If the token count is less than or equal to one, then we have called a function with no arguments.

         // At this point iVar now contains the number of arguments that we have for the function, we also know the functions
         // name, and name length, attempt to find a function to call.
         if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
	      {
#ifdef MPARSER_VERBOSE
	      	printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
				return SC_SYNTAX_ERROR;
			}

         // Check the correctness of the expression - we should not get an operand in the post operand state.
         if ( cExpState == EXP_STATE_POST )
         	return SC_SYNTAX_ERROR;

         // Shift onto the operand stack a temporary value - since this is a constant
         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[cOperandStackTop]]);

         // Load the function name from xmem into root memory.
         xmem2root( g_oStringBuffer, ppFuncNameLoc, iFuncNameLen );

         // Call the function
         if ( (cState = CallFunction( pServer, pOperandStack[cOperandStackTop], iAllocatedFunctionArguments, g_oStringBuffer, iFuncNameLen, iVar )) != SC_OK )
         	return cState;

         // Increment the top of the operand stack by 1
	      cOperandStackTop++;

         // Change the expression state to post operand state.
         cExpState = EXP_STATE_POST;
      	break;

      case TYPE_LONG:	// Long constant - push onto operand stack.
         if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
	      {
#ifdef MPARSER_VERBOSE
	      	printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
				return SC_SYNTAX_ERROR;
			}

         // Check the correctness of the expression - we should not get an operand in the post operand state.
         if ( cExpState == EXP_STATE_POST )
         	return SC_SYNTAX_ERROR;

         // Shift onto the operand stack a temporary value - since this is a constant
         // pOperandStack[cOperandStackTop] = &oOperandStack[cOperandStackTop];
         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[cOperandStackTop]]);


			// Shift onto the operand stack.
	      pOperandStack[cOperandStackTop]->cVarType = VARTYPE_INT;
         pOperandStack[cOperandStackTop]->VarData.iIntValue = T1.TokenData.lValue;

	      // Increment the top of the operand stack by 1
	      cOperandStackTop++;

         // Change the expression state to post operand state.
         cExpState = EXP_STATE_POST;
      	break;

#if 0
      case TYPE_FLOAT:	// Long constant - push onto operand stack.
         if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
	      {
#ifdef MPARSER_VERBOSE
	      	printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
				return SC_SYNTAX_ERROR;
			}

         // Check the correctness of the expression - we should not get an operand in the post operand state.
         if ( cExpState == EXP_STATE_POST )
         	return SC_SYNTAX_ERROR;

         // Shift onto the operand stack a temporary value - since this is a constant
         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[cOperandStackTop]]);


			// Shift onto the operand stack.
	      pOperandStack[cOperandStackTop]->cVarType = VARTYPE_FLOAT;
         pOperandStack[cOperandStackTop]->VarData.fFloatValue = T1.TokenData.fValue;

	      // Increment the top of the operand stack by 1
	      cOperandStackTop++;

         // Change the expression state to post operand state.
         cExpState = EXP_STATE_POST;
      	break;
#endif

   	case TYPE_STRING:	// String constant - push onto operand stack.
         if ( cOperandStackTop >= EXP_OPERAND_STACK_SIZE )
	      {
#ifdef MPARSER_VERBOSE
	      	printf( "[MPARSER_VERBOSE] Error - Exceeded maximum size of operand stack in DoExpressionParser()\n" );
#endif
				return SC_SYNTAX_ERROR;
			}

         // Check the correctness of the expression - we should not get an operand in the post operand state.
         if ( cExpState == EXP_STATE_POST )
         	return SC_SYNTAX_ERROR;

         // Shift onto the operand stack a temporary value - since this is a constant
         pOperandStack[cOperandStackTop] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[cOperandStackTop]]);

			// Shift onto the operand stack.
	      pOperandStack[cOperandStackTop]->cVarType = VARTYPE_STRING;

         // Copy the strings memory into the string buffer and append a null terminator to format it correctly.
         strncpy( g_oStringBuffer, T1.sStringBuffer, T1.TokenData.ucStringLen );

         if ( SaveVariableString( pServer, pOperandStack[cOperandStackTop], g_oStringBuffer, T1.TokenData.ucStringLen ) == FALSE )
         	return SC_INTERNALERROR;

	      // Increment the top of the operand stack by 1
	      cOperandStackTop++;

         // Change the expression state to post operand state.
         cExpState = EXP_STATE_POST;

      	break;

      default:
#ifdef MPARSER_VERBOSE
			printf( "[MPARSER_VERBOSE] Error - default case in DoExpressionParser()\n" );
#endif
         break;
      }



   }

   return SC_SYNTAX_ERROR;  // End of file before we were able to finish reading expression
}




/* START FUNCTION DESCRIPTION ********************************************
DoExpressionReduce                  <MAIN_PARSER.LIB>

SYNTAX: int DoExpressionReduce( HttpServerStruct *pServer, PHPOperator *pOperatorStack, PHPVariable **pOperandStack, int *cOperatorStackTop, int *cOperandStackTop, int iAllocatedOperandStack[] )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	PHPVariable that is to be converted to boolean value

RETURN VALUE:  Performs a type conversion of the current value and returns a int 
					which represents the boolean value.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoExpressionReduce( HttpServerStruct *pServer, PHPOperator *pOperatorStack, PHPVariable **pOperandStack, int *cOperatorStackTop, int *cOperandStackTop, int iAllocatedOperandStack[] )
{
   auto int cOpType;
   auto int cOperandCount;
   auto PHPVariable oResultVariable;
   auto PHPVariable *pLVal, *pRVal;

   char szReturnTemporary[PHP_MAX_STRINGLENGTH+1];
   oResultVariable.iVNum = VAR_VNUM_TEMPORARY;
   oResultVariable.VarData.ppStringValue = szReturnTemporary;

	// Reduction involves removing the top operator and from that operator determining the number of
   // operands that are referenced by that operand -  this can be done by the operators type,
   // infix/postfix/prefix/etc.

   // Do we have an empty operator stack to work with?
   if ( (*cOperatorStackTop) == 0 )
   	return SC_SYNTAX_ERROR;

   // Decrement the top of the operator stack... since this is the operator we will be dealing with.
   (*cOperatorStackTop)--;

   // Get the type of the operator we are reducing and the number of operands for it.
   cOpType = pOperatorStack[(*cOperatorStackTop)].cOperatorType;
   switch( cOpType )
   {
   case EXP_OPTYPE_PREFIX:
   case EXP_OPTYPE_POSTFIX:
   	cOperandCount = 1;
      break;

   case EXP_OPTYPE_INFIX:
   	cOperandCount = 2;
      break;

	case EXP_OPTYPE_CONFIX_OPEN:
	case EXP_OPTYPE_CONFIX_CLOSE:
   case EXP_OPTYPE_FUNC_OPEN:
   case EXP_OPTYPE_FUNC_CLOSE:
   	return SC_OK;
      break;

	case EXP_OPTYPE_NOTFOUND:
   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in DoExpressionReduce()\n" );
#endif
		return SC_SYNTAX_ERROR;
   }

   if ( ((*cOperandStackTop) - cOperandCount) < 0 )
   {
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - too few operands on the operand stack in DoExpressionReduce()\n" );
#endif
   	return SC_SYNTAX_ERROR;
   }

   (*cOperandStackTop)--;
   pRVal = pOperandStack[(*cOperandStackTop)];

   if ( cOperandCount == 2 )
   {
      (*cOperandStackTop)--;
   	pLVal = pOperandStack[(*cOperandStackTop)];
   }
   else
   	pLVal = NULL;

#ifdef MPARSER_VERBOSE
	printf( "MPARSRE: OPERATOR [%d]\n", pOperatorStack[(*cOperatorStackTop)].ucOperatorSymbol );
#endif

   // This is where we perform the actual functions, use PHP rules for converting.
   switch( pOperatorStack[(*cOperatorStackTop)].ucOperatorSymbol )
   {
   case TOKEN_NOT:	// Not always uses an integer - so we must convert to an integer.
      oResultVariable.VarData.iIntValue = !ConvertVariableToInt( pRVal );
      oResultVariable.cVarType = VARTYPE_INT;
      break;

   case TOKEN_TILDA: // One's complement - always uses an integer
   	oResultVariable.VarData.iIntValue = ~ConvertVariableToInt( pRVal );
      oResultVariable.cVarType = VARTYPE_INT;
      break;

   case TOKEN_SUBTRACT:	// Subtraction or Negate
      if ( cOperandCount == 2 )
      {
      	// Check to see if we need to promote to float.
#if ENABLE_FLOAT
         if ( pLVal->cVarType == VARTYPE_FLOAT ||
         	  pRVal->cVarType == VARTYPE_FLOAT )
         {
         	oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) - ConvertVariableToFloat( pRVal ));
            oResultVariable.cVarType = VARTYPE_FLOAT;
         }
         else // Convert to int
#endif
         {
            oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) - ConvertVariableToInt( pRVal ));
            oResultVariable.cVarType = VARTYPE_INT;
         }
      }
      else
      {
#if ENABLE_FLOAT
      	if ( pRVal->cVarType == VARTYPE_FLOAT )
         {
            // Convert to int.
         	oResultVariable.VarData.fFloatValue = -(pRVal->VarData.fFloatValue);
         	oResultVariable.cVarType = VARTYPE_FLOAT;
         }
         else
#endif
         {
      		// Convert to int.
         	oResultVariable.VarData.iIntValue = -(ConvertVariableToInt( pRVal ));
         	oResultVariable.cVarType = VARTYPE_INT;
         }
      }
      break;

   case TOKEN_ADD:      // Addition
      // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      if ( pLVal->cVarType == VARTYPE_FLOAT ||
           pRVal->cVarType == VARTYPE_FLOAT )
      {
         oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) + ConvertVariableToFloat( pRVal ));
         oResultVariable.cVarType = VARTYPE_FLOAT;
      }
      else // Convert to int
#endif
      {
         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) + ConvertVariableToInt( pRVal ));
         oResultVariable.cVarType = VARTYPE_INT;
      }
      break;

   case TOKEN_MOD:		// Modulus
   	// Modulus only works on integers - convert to int
      if ( ConvertVariableToInt( pRVal ) == 0 ) // Check divide by zero
      	return SC_DIVIDEBYZERO_ERROR;

      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) % ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;
      break;

   case TOKEN_STAR:     // Multiplication
      // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      if ( pLVal->cVarType == VARTYPE_FLOAT ||
           pRVal->cVarType == VARTYPE_FLOAT )
      {
         oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) * ConvertVariableToFloat( pRVal ));
         oResultVariable.cVarType = VARTYPE_FLOAT;
      }
      else // Convert to int
#endif
      {
         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) * ConvertVariableToInt( pRVal ));
         oResultVariable.cVarType = VARTYPE_INT;
      }
      break;

   case TOKEN_FORWARD_SLASH:	// Division
#if ENABLE_FLOAT
      // Division always returns a float
      if ( ConvertVariableToFloat( pRVal ) == 0.0 )
      	return SC_DIVIDEBYZERO_ERROR;

      oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) / ConvertVariableToFloat( pRVal ));
      oResultVariable.cVarType = VARTYPE_FLOAT;
      break;
#else
	if ( ConvertVariableToInt( pRVal ) == 0 )
		return SC_DIVIDEBYZERO_ERROR;

	oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) / ConvertVariableToInt( pRVal ));
	oResultVariable.cVarType = VARTYPE_INT;
	break;
#endif

   case TOKEN_PERIOD:	// Concatenate two strings together.
   	ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      strncat( g_oStringBuffer, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );
      // strncpy( g_oStringBuffer, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );
      SaveVariableString( pServer, &oResultVariable, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   	break;

   case TOKEN_ASSIGNMENT:	// assignment
   	CopyPHPVariable( pServer, pLVal, pRVal );
      CopyPHPVariable( pServer, &oResultVariable, pRVal );
      break;

   case TOKEN_EQUALTO:				// ==
   	oResultVariable.cVarType = VARTYPE_BOOLEAN;
      oResultVariable.VarData.iIntValue = DoNormalComparison( pServer, pLVal, pRVal );
      break;

   case TOKEN_NOT_EQUAL:			// !=
   	oResultVariable.cVarType = VARTYPE_BOOLEAN;
      oResultVariable.VarData.iIntValue = DoNormalComparison( pServer, pLVal, pRVal );

      if ( oResultVariable.VarData.iIntValue == FALSE )
      	oResultVariable.VarData.iIntValue = TRUE;
      else
      	oResultVariable.VarData.iIntValue = FALSE;
      break;

   case TOKEN_EQUAL_IDENT:			// ===
   	oResultVariable.cVarType = VARTYPE_BOOLEAN;
      oResultVariable.VarData.iIntValue = DoStrictComparison( pServer, pLVal, pRVal );
      break;

   case TOKEN_NOT_EQUAL_IDENT:	// !==
      oResultVariable.cVarType = VARTYPE_BOOLEAN;
      oResultVariable.VarData.iIntValue = DoStrictComparison( pServer, pLVal, pRVal );

      if ( oResultVariable.VarData.iIntValue == FALSE )
      	oResultVariable.VarData.iIntValue = TRUE;
      else
      	oResultVariable.VarData.iIntValue = FALSE;
      break;

   case TOKEN_ADD_EQUAL:					// +=
#if ENABLE_FLOAT
   	// Check to see if we need to promote to float.
      if ( pLVal->cVarType == VARTYPE_FLOAT ||
           pRVal->cVarType == VARTYPE_FLOAT )
      {
         oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) + ConvertVariableToFloat( pRVal ));
         oResultVariable.cVarType = VARTYPE_FLOAT;
      }
      else // Convert to int
#endif
      {
         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) + ConvertVariableToInt( pRVal ));
         oResultVariable.cVarType = VARTYPE_INT;
      }
      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_SUBTRACT_EQUAL:				// -=
#if ENABLE_FLOAT
      // Check to see if we need to promote to float.
      if ( pLVal->cVarType == VARTYPE_FLOAT ||
           pRVal->cVarType == VARTYPE_FLOAT )
      {
         oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) - ConvertVariableToFloat( pRVal ));
         oResultVariable.cVarType = VARTYPE_FLOAT;
      }
      else // Convert to int
#endif
      {
         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) - ConvertVariableToInt( pRVal ));
         oResultVariable.cVarType = VARTYPE_INT;
      }
      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_STAR_EQUAL:					// *=
   	// Check to see if we need to promote to float.
#if ENABLE_FLOAT
      if ( pLVal->cVarType == VARTYPE_FLOAT ||
           pRVal->cVarType == VARTYPE_FLOAT )
      {
         oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) * ConvertVariableToFloat( pRVal ));
         oResultVariable.cVarType = VARTYPE_FLOAT;
      }
      else // Convert to int
#endif
      {
         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) * ConvertVariableToInt( pRVal ));
         oResultVariable.cVarType = VARTYPE_INT;
      }
      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_FORWARD_SLASH_EQUAL:		// /=
#if ENABLE_FLOAT
      // Division always returns a float
      if ( ConvertVariableToFloat( pRVal ) == 0.0 )
      	return SC_DIVIDEBYZERO_ERROR;

      oResultVariable.VarData.fFloatValue = (ConvertVariableToFloat( pLVal ) / ConvertVariableToFloat( pRVal ));
      oResultVariable.cVarType = VARTYPE_FLOAT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;
#else
	if ( ConvertVariableToInt( pRVal ) == 0 )
		return SC_DIVIDEBYZERO_ERROR;

	oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) / ConvertVariableToInt( pRVal ));
	oResultVariable.cVarType = VARTYPE_INT;

	CopyPHPVariable( pServer, pLVal, &oResultVariable );
	break;
#endif

   case TOKEN_PERIOD_EQUAL:   // .=
      ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      strncat( g_oStringBuffer, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );

      SaveVariableString( pServer, &oResultVariable, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      SaveVariableString( pServer, pLVal, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      break;

   case TOKEN_MOD_EQUAL:	// %=
      // Modulus only works on integers - convert to int
      if ( ConvertVariableToInt( pRVal ) == 0 ) // Check divide by zero
      	return SC_DIVIDEBYZERO_ERROR;

      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) % ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_AMPERSAND_EQUAL: // &=
   	// Always convert to int - logical & only makes sense on integer values
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) & ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;


   case TOKEN_OR_EQUAL:		// |=
      // Always convert to int - logical | only makes sense on integer values
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) | ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_XOR_EQUAL:	// ^=
      // Always convert to int - logical ^ only makes sense on integer values
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) ^ ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_SHIFT_LEFT_EQUAL:			// <<=
      // Always convert to int - shifting only makes sense on integer values.
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) << ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_SHIFT_RIGHT_EQUAL:			// >>=
   	// Always convert to int - shifting only makes sense on integer values.
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) >> ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      CopyPHPVariable( pServer, pLVal, &oResultVariable );
      break;

   case TOKEN_SHIFT_LEFT:		// << Logical shift left
      // Always convert to int - shifting only makes sense on integer values.
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) << ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      break;

   case TOKEN_SHIFT_RIGHT:		// >> Logical shift right
      // Always convert to int - shifting only makes sense on integer values.
      oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pLVal ) >> ConvertVariableToInt( pRVal ));
      oResultVariable.cVarType = VARTYPE_INT;

      break;

   case TOKEN_LESS_THAN:         // <
      DoLessThanComparison( pServer, &oResultVariable, pLVal, pRVal );
   	break;

   case TOKEN_LESS_EQUAL:        // <=
   	DoLessThanComparison( pServer, &oResultVariable, pLVal, pRVal );
      CopyPHPVariable( pServer, pLVal, &oResultVariable );
   	break;

   case TOKEN_GREATER_THAN:      // >
      DoGreaterThanComparison( pServer, &oResultVariable, pLVal, pRVal );
   	break;

   case TOKEN_GREATER_EQUAL:     // >=
   	DoGreaterThanComparison( pServer, &oResultVariable, pLVal, pRVal );
      CopyPHPVariable( pServer, pLVal, &oResultVariable );
   	break;

   case TOKEN_INCREMENT:			// ++
   	if ( cOpType == EXP_OPTYPE_PREFIX )
      {
         // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      	if ( pRVal->cVarType == VARTYPE_FLOAT )
	      {
	         oResultVariable.VarData.fFloatValue = pRVal->VarData.fFloatValue + 1.0;
	         oResultVariable.cVarType = VARTYPE_FLOAT;
	      }
	      else // Convert to int
#endif
	      {
	         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pRVal ) + 1);
	         oResultVariable.cVarType = VARTYPE_INT;
	      }
         CopyPHPVariable( pServer, pRVal, &oResultVariable ); // Store back the incremented value.
      }
      else if ( cOpType == EXP_OPTYPE_POSTFIX )
      {
         // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      	if ( pRVal->cVarType == VARTYPE_FLOAT )
	      {
	         oResultVariable.VarData.fFloatValue = pRVal->VarData.fFloatValue;
	         oResultVariable.cVarType = VARTYPE_FLOAT;

            pRVal->VarData.fFloatValue += 1.0;
	      }
	      else // Convert to int
#endif
	      {
	         oResultVariable.VarData.iIntValue = ConvertVariableToInt( pRVal );
	         oResultVariable.cVarType = VARTYPE_INT;

            pRVal->VarData.iIntValue = ConvertVariableToInt( pRVal ) + 1;
            pRVal->cVarType = VARTYPE_INT;
	      }
      }
      break;

   case TOKEN_DECREMENT:			// --
      if ( cOpType == EXP_OPTYPE_PREFIX )
      {
         // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      	if ( pRVal->cVarType == VARTYPE_FLOAT )
	      {
	         oResultVariable.VarData.fFloatValue = pRVal->VarData.fFloatValue - 1.0;
	         oResultVariable.cVarType = VARTYPE_FLOAT;
	      }
	      else // Convert to int
#endif
	      {
	         oResultVariable.VarData.iIntValue = (ConvertVariableToInt( pRVal ) - 1);
	         oResultVariable.cVarType = VARTYPE_INT;
	      }
         CopyPHPVariable( pServer, pRVal, &oResultVariable ); // Store back the incremented value.
      }
      else if ( cOpType == EXP_OPTYPE_POSTFIX )
      {
         // Check to see if we need to promote to float.
#if ENABLE_FLOAT
      	if ( pRVal->cVarType == VARTYPE_FLOAT )
	      {
	         oResultVariable.VarData.fFloatValue = pRVal->VarData.fFloatValue;
	         oResultVariable.cVarType = VARTYPE_FLOAT;

            pRVal->VarData.fFloatValue -= 1.0;
	      }
	      else // Convert to int
#endif
	      {
	         oResultVariable.VarData.iIntValue = ConvertVariableToInt( pRVal );
	         oResultVariable.cVarType = VARTYPE_INT;

            pRVal->VarData.iIntValue = ConvertVariableToInt( pRVal ) - 1;
            pRVal->cVarType = VARTYPE_INT;
	      }
      }
      break;

      case TOKEN_CAST_BOOLEAN:				// (boolean)
      	oResultVariable.VarData.iIntValue = ConvertVariableToBoolean( pRVal );
         oResultVariable.cVarType = VARTYPE_BOOLEAN;

         break;

#if ENABLE_FLOAT
   	case TOKEN_CAST_FLOAT:					// (float)
      	oResultVariable.VarData.fFloatValue = ConvertVariableToFloat( pRVal );
         oResultVariable.cVarType = VARTYPE_FLOAT;

         break;
#endif

   	case TOKEN_CAST_STRING:					// (string)
         ConvertVariableToString( pRVal, g_oStringBuffer );
      	SaveVariableString( pServer, &oResultVariable, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

         break;

   	case TOKEN_CAST_CHAR:					// (char)
      	oResultVariable.VarData.iIntValue = ConvertVariableToInt( pRVal );
         oResultVariable.cVarType = VARTYPE_INT;

      	break;

   	case TOKEN_CAST_INT:						// (int)
      	oResultVariable.VarData.iIntValue = ConvertVariableToInt( pRVal );
         oResultVariable.cVarType = VARTYPE_INT;

         break;

      case TOKEN_LOGICAL_AND:			// &&
   	case TOKEN_LOGICAL_AND2:		// and
      	oResultVariable.VarData.iIntValue = (ConvertVariableToBoolean( pLVal ) && ConvertVariableToBoolean( pRVal ));
         oResultVariable.cVarType = VARTYPE_BOOLEAN;
      	break;

      case TOKEN_LOGICAL_OR:			// ||
   	case TOKEN_LOGICAL_OR2:			// or
      	oResultVariable.VarData.iIntValue = (ConvertVariableToBoolean( pLVal ) || ConvertVariableToBoolean( pRVal ));
         oResultVariable.cVarType = VARTYPE_BOOLEAN;
   		break;

      case TOKEN_LOGICAL_XOR2:		// xor
         DoXORComparison( pServer, &oResultVariable, pLVal, pRVal );
      	break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - Unrecognized operator in DoExpressionReduce()\n" );
#endif
		return SC_SYNTAX_ERROR;
      break;
   }

   // We need to set up the top of the operand stack to point to a temporary value, so point it
   // back to the allocated operand stack (we cannot be sure that this is a temporary value so we
   // force it to be one).

#ifdef MPARSER_VERBOSE
	printf( "MPARSER:: DoReduceExpression:: Saving result to operand stack %d\n", (*cOperandStackTop) );
#endif

   pOperandStack[(*cOperandStackTop)] = &(pServer->php_interpreter.oVariableTable[iAllocatedOperandStack[(*cOperandStackTop)]]);

   // Now push the result in oResultVariable back onto the operand stack
   CopyPHPVariable( pServer, pOperandStack[(*cOperandStackTop)], &oResultVariable );

#ifdef MPARSER_VERBOSE
		printf( "MPARSER:: Reduce complete!\n" );
#endif

   (*cOperandStackTop)++;

   return SC_OK;
}


/* START FUNCTION DESCRIPTION ********************************************
ConvertVariableToBoolean                  <MAIN_PARSER.LIB>

SYNTAX: int ConvertVariableToBoolean( PHPVariable *pVar )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	PHPVariable that is to be converted to boolean value

RETURN VALUE:  Performs a type conversion of the current value and returns a int 
					which represents the boolean value.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int ConvertVariableToBoolean( PHPVariable *pVar )
{
   switch( pVar->cVarType )
   {
   case VARTYPE_BOOLEAN:
   	if ( pVar->VarData.iIntValue == 0 )
      	return FALSE;
      else
      	return TRUE;
      break;

   case VARTYPE_INT:
   	if ( pVar->VarData.iIntValue == 0 )
      	return FALSE;
      else
      	return TRUE;
      break;

#if 0
	case VARTYPE_FLOAT:
      if ( pVar->VarData.fFloatValue == 0.0 )
      	return FALSE;
      else
      	return TRUE;
      break;
#endif

   case VARTYPE_STRING:
   	// First copy the string from xmem into root code to analyze it
      LoadVariableString( pVar, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      if ( g_oStringBuffer[0] == '\0' ||
      	  (g_oStringBuffer[0] == '0' && g_oStringBuffer[1] == '\0') )
      	return FALSE;
      else
       	return TRUE;
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in ConvertVariableToBoolean()\n" );
#endif
		break;
   }

   return FALSE;
}

/* START FUNCTION DESCRIPTION ********************************************
ConvertVariableToInt                  <MAIN_PARSER.LIB>

SYNTAX: int ConvertVariableToInt( PHPVariable *pVar )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	PHPVariable that is to be converted to int value

RETURN VALUE:  Performs a type conversion of the current value and returns a int 
					which represents the integer value.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int ConvertVariableToInt( PHPVariable *pVar )
{
   switch( pVar->cVarType )
   {
   case VARTYPE_BOOLEAN:
   case VARTYPE_INT:
   	return pVar->VarData.iIntValue;
      break;

#if 0
	case VARTYPE_FLOAT:
   	return ((int)pVar->VarData.fFloatValue);
      break;
#endif

   case VARTYPE_STRING:
      // First copy the string from xmem into root code to analyze it
      LoadVariableString( pVar, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      return atoi( g_oStringBuffer );
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in ConvertVariableToInt()\n" );
#endif
		break;
   }

   return FALSE;
}

/* START FUNCTION DESCRIPTION ********************************************
ConvertVariableToFloat                  <MAIN_PARSER.LIB>

SYNTAX: int ConvertVariableToFloat( PHPVariable *pVar )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	PHPVariable that is to be converted to float value

RETURN VALUE:  Performs a type conversion of the current value and returns a float
					which represents the float value.

SEE ALSO:

END DESCRIPTION **********************************************************/
#if 0
_mparser_nodebug float ConvertVariableToFloat( PHPVariable *pVar )
{
   switch( pVar->cVarType )
   {
   case VARTYPE_BOOLEAN:
   case VARTYPE_INT:
   	return ((float)pVar->VarData.iIntValue);
      break;

	case VARTYPE_FLOAT:
   	return pVar->VarData.fFloatValue;
      break;

   case VARTYPE_STRING:
      // First copy the string from xmem into root code to analyze it
      LoadVariableString( pVar, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      return atof( g_oStringBuffer );
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in ConvertVariableToInt()\n" );
#endif
		break;
   }

   return FALSE;
}
#endif



/* START FUNCTION DESCRIPTION ********************************************
ConvertVariableToString                  <MAIN_PARSER.LIB>

SYNTAX: int ConvertVariableToString( PHPVariable *pVar, char *pDest )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	PHPVariable that is to be converted to float value
PARAMETER2: The destination address of the string

RETURN VALUE:  Performs a type conversion of the current value and returns a float
					which represents the float value.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int ConvertVariableToString( PHPVariable *pVar, char *pDest )
{
   switch( pVar->cVarType )
   {
   case VARTYPE_BOOLEAN:
   	if ( pVar->VarData.iIntValue == TRUE )
      	strcpy( pDest, "TRUE" );
      else
      	strcpy( pDest, "FALSE" );
      return TRUE;
      break;

   case VARTYPE_INT:
      sprintf( pDest, "%d", pVar->VarData.iIntValue );
      return TRUE;
      break;

#if 0
	case VARTYPE_FLOAT:
   	sprintf( pDest, "%f", pVar->VarData.fFloatValue );
   	return TRUE;
      break;
#endif

   case VARTYPE_STRING:
      // First copy the string from xmem into root code to analyze it
		strcpy( pDest, pVar->VarData.ppStringValue );
      // LoadVariableString( pVar, pDest, PHP_MAX_STRINGLENGTH );
      return TRUE;
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in ConvertVariableToInt()\n" );
#endif
		break;
   }

   return FALSE;
}

/* START FUNCTION DESCRIPTION ********************************************
GetOperatorType                  <MAIN_PARSER.LIB>

SYNTAX: int GetOperatorType( unsigned int ucSymbol )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	The symbol that is to be checked as an operator and the type determined.

RETURN VALUE:  Returns the type of operator this symbol is defined by the EXP_OPTYPE_*
					defines.  Returns EXP_OPTYPE_NOTFOUND if it is not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int GetOperatorType( unsigned int ucSymbol, int cExpState )
{
   switch( ucSymbol )
   {
   case TOKEN_NOT:							// !
   case TOKEN_TILDA:							// ~
   case TOKEN_CAST_BOOLEAN:				// (boolean)
   case TOKEN_CAST_FLOAT:					// (float)
   case TOKEN_CAST_STRING:					// (string)
   case TOKEN_CAST_CHAR:					// (char)
   case TOKEN_CAST_INT:						// (int)
   	return EXP_OPTYPE_PREFIX;
      break;

   case TOKEN_INCREMENT:
   case TOKEN_DECREMENT:
   	if ( cExpState == EXP_STATE_PRE ) // We are in the pre-operand state (ie haven't seen an operand yet) - thus a prefix
      	return EXP_OPTYPE_PREFIX;
      else 										 // Else we are in the postfix state (ie after we have seen an operand)
      	return EXP_OPTYPE_POSTFIX;
      break;

   case TOKEN_SUBTRACT:
   	if ( cExpState == EXP_STATE_PRE ) // We are in the pre-operand state - thus this is a prefix operator
      	return EXP_OPTYPE_PREFIX;
      else
      	return EXP_OPTYPE_INFIX;	// Else subtract refers to an infix operator
      break;

   case TOKEN_MOD:							// %
   case TOKEN_ADD:      					// +
   case TOKEN_STAR:     					// *
   case TOKEN_FORWARD_SLASH:				// /
   case TOKEN_PERIOD:						// .
   case TOKEN_ASSIGNMENT:					// =
   case TOKEN_EQUALTO: 						// ==
   case TOKEN_NOT_EQUAL:      			// !=
   case TOKEN_EQUAL_IDENT:					// ===
   case TOKEN_NOT_EQUAL_IDENT:			// !==

   case TOKEN_ADD_EQUAL:					// +=
   case TOKEN_SUBTRACT_EQUAL:				// -=
   case TOKEN_STAR_EQUAL:					// *=
   case TOKEN_FORWARD_SLASH_EQUAL:		// /=
   case TOKEN_PERIOD_EQUAL:     			// .=
   case TOKEN_MOD_EQUAL:	  				// %=
   case TOKEN_AMPERSAND_EQUAL:  			// &=
   case TOKEN_OR_EQUAL:			  			// |=
   case TOKEN_XOR_EQUAL:	  				// ^=
   case TOKEN_SHIFT_LEFT_EQUAL:			// <<=
   case TOKEN_SHIFT_RIGHT_EQUAL:			// >>=
   case TOKEN_SHIFT_LEFT:		  			// << Logical shift left
   case TOKEN_SHIFT_RIGHT:					// >> Logical shift right
   case TOKEN_LESS_THAN:           		// <
   case TOKEN_LESS_EQUAL:          		// <=
   case TOKEN_GREATER_THAN:        		// >
   case TOKEN_GREATER_EQUAL:     		// >=
   case TOKEN_LOGICAL_AND:			  		// &&
   case TOKEN_LOGICAL_OR:			 		// ||
   case TOKEN_LOGICAL_AND2:		  		// and
   case TOKEN_LOGICAL_OR2:			  		// or
   case TOKEN_LOGICAL_XOR2:				// xor
   	return EXP_OPTYPE_INFIX;
      break;



   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in GetOperatorType()\n" );
#endif
		return EXP_OPTYPE_NOTFOUND;
		break;
   }

   return EXP_OPTYPE_NOTFOUND;
}


/* START FUNCTION DESCRIPTION ********************************************
GetOperatorPrecedence                  <MAIN_PARSER.LIB>

SYNTAX: int GetOperatorPrecedence( unsigned int ucSymbol )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	The symbol that is to be checked as an operator.

RETURN VALUE:  Returns the precedence of the operator, or -1 if not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int GetOperatorPrecedence( unsigned int ucSymbol )
{
   switch( ucSymbol )
   {
   case TOKEN_NOT:		  					// Logical Not
   case TOKEN_TILDA:	 		  				// One's complement
   case TOKEN_INCREMENT:	  				// ++
   case TOKEN_DECREMENT:	  				// --
   case TOKEN_CAST_BOOLEAN:				// (boolean)
   case TOKEN_CAST_FLOAT:					// (float)
   case TOKEN_CAST_STRING:					// (string)
   case TOKEN_CAST_CHAR:					// (char)
   case TOKEN_CAST_INT:						// (int)
   	return 120;
      break;

   case TOKEN_STAR:	 	 					// Multiplication
   case TOKEN_MOD:     						// Modulus
   case TOKEN_FORWARD_SLASH:	  			// Division
   	return 115;
      break;

   case TOKEN_SUBTRACT:						// -
   case TOKEN_ADD:							// +
   case TOKEN_PERIOD:						// .
   	return 110;
      break;

   case TOKEN_SHIFT_LEFT:			  		// << Logical shift left
   case TOKEN_SHIFT_RIGHT:		  			// >> Logical shift right
   	return 105;
   	break;

   case TOKEN_LESS_THAN:           		// <
   case TOKEN_LESS_EQUAL:          		// <=
   case TOKEN_GREATER_THAN:        		// >
   case TOKEN_GREATER_EQUAL:       		// >=
   	return 100;
      break;


   case TOKEN_EQUALTO:						// ==
   case TOKEN_NOT_EQUAL:           		// !=
   case TOKEN_EQUAL_IDENT:			  		// ===
   case TOKEN_NOT_EQUAL_IDENT:	  		// !==
   	return 95;
      break;

   case TOKEN_LOGICAL_AND:					// &&
   	return 75;
      break;

   case TOKEN_LOGICAL_OR:					// ||
   	return 70;
      break;


   case TOKEN_ASSIGNMENT:					// =
   case TOKEN_ADD_EQUAL:					// +=
   case TOKEN_SUBTRACT_EQUAL:				// -=
   case TOKEN_STAR_EQUAL:					// *=
   case TOKEN_FORWARD_SLASH_EQUAL:		// /=
   case TOKEN_PERIOD_EQUAL:     			// .=
   case TOKEN_MOD_EQUAL:	  				// %=
   case TOKEN_AMPERSAND_EQUAL:  			// &=
   case TOKEN_OR_EQUAL:		  				// |=
   case TOKEN_XOR_EQUAL:	  				// ^=
   case TOKEN_SHIFT_LEFT_EQUAL:			// <<=
   case TOKEN_SHIFT_RIGHT_EQUAL:			// >>=
   	return 60;
      break;

   case TOKEN_LOGICAL_AND2:				// and
   	return 50;
      break;

   case TOKEN_LOGICAL_XOR2:				// xor
   	return 45;
      break;

   case TOKEN_LOGICAL_OR2:					// or
   	return 40;
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in GetOperatorPrecedence()\n" );
#endif
		return -1;
		break;
   }

   return -1;
}

/* START FUNCTION DESCRIPTION ********************************************
GetOperatorAssociativity                  <MAIN_PARSER.LIB>

SYNTAX: int GetOperatorAssociativity( unsigned int ucSymbol )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	The symbol that is to be checked as an operator.

RETURN VALUE:  Returns the associativity of the operator, or -1 if not found.

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int GetOperatorAssociativity( unsigned int ucSymbol )
{
   switch( ucSymbol )
   {
   case TOKEN_NOT:							// Logical Not (!)
   case TOKEN_TILDA:							// One's complement (~)
	case TOKEN_ASSIGNMENT:					// Assignment (=)
   case TOKEN_ADD_EQUAL:					// +=
   case TOKEN_SUBTRACT_EQUAL:				// -=
   case TOKEN_STAR_EQUAL:					// *=
   case TOKEN_FORWARD_SLASH_EQUAL:		// /=
   case TOKEN_PERIOD_EQUAL:   			// .=
   case TOKEN_MOD_EQUAL:					// %=
   case TOKEN_AMPERSAND_EQUAL: 			// &=
   case TOKEN_OR_EQUAL:						// |=
   case TOKEN_XOR_EQUAL:					// ^=
   case TOKEN_SHIFT_LEFT_EQUAL:			// <<=
   case TOKEN_SHIFT_RIGHT_EQUAL:			// >>=
   case TOKEN_INCREMENT:					// ++
   case TOKEN_DECREMENT:					// --
   case TOKEN_CAST_BOOLEAN:				// (boolean)
   case TOKEN_CAST_FLOAT:					// (float)
   case TOKEN_CAST_STRING:					// (string)
   case TOKEN_CAST_CHAR:					// (char)
   case TOKEN_CAST_INT:						// (int)
   	return EXP_ASSOC_RIGHT;
      break;

   case TOKEN_STAR:					// *
   case TOKEN_MOD:   				// %
   case TOKEN_FORWARD_SLASH:		// /
  	case TOKEN_SUBTRACT:				// -
   case TOKEN_ADD:           		// +
   case TOKEN_PERIOD:         	// .
   case TOKEN_SHIFT_LEFT:			// << Logical shift left
   case TOKEN_SHIFT_RIGHT:			// >> Logical shift right
   case TOKEN_LOGICAL_AND:			// &&
   case TOKEN_LOGICAL_OR:			// ||
   case TOKEN_LOGICAL_AND2:		// and
   case TOKEN_LOGICAL_OR2:			// or
   case TOKEN_LOGICAL_XOR2:		// xor
   	return EXP_ASSOC_LEFT;
      break;

   case TOKEN_EQUALTO:				// ==
   case TOKEN_NOT_EQUAL:         // !=
   case TOKEN_EQUAL_IDENT:			// ===
   case TOKEN_NOT_EQUAL_IDENT:	// !==
   case TOKEN_LESS_THAN:         // <
   case TOKEN_LESS_EQUAL:        // <=
   case TOKEN_GREATER_THAN:      // >
   case TOKEN_GREATER_EQUAL:     // >=
   	return EXP_ASSOC_NONE;
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in GetOperatorPrecedence()\n" );
#endif
		return -1;
		break;
   }

   return -1;
}



/* START FUNCTION DESCRIPTION ********************************************
CopyPHPVariable                  <MAIN_PARSER.LIB>

SYNTAX: void CopyPHPVariable( PHPVariable *pDest, PHPVariable *pSrc )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	The destination variable to be copied into.
PARAMETER2: The source variable to be copied from.

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug void CopyPHPVariable( HttpServerStruct *pServer, PHPVariable *pDest, PHPVariable *pSrc )
{
	// Copy the source to the destination
   pDest->cVarType = pSrc->cVarType;

   switch( pSrc->cVarType )
   {
   case VARTYPE_BOOLEAN:
   case VARTYPE_INT:
   	pDest->VarData.iIntValue = pSrc->VarData.iIntValue;
      break;

#if 0
   case VARTYPE_FLOAT:
   	pDest->VarData.fFloatValue = pSrc->VarData.fFloatValue;
      break;
#endif

   case VARTYPE_STRING:
      // Load and then save
      LoadVariableString( pSrc, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      SaveVariableString( pServer, pDest, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
      break;

   default:
#ifdef MPARSER_VERBOSE
		printf( "[MPARSER_VERBOSE] Error - default case in CopyPHPVariable()\n" );
#endif
		break;
   }

   return;
}


/* START FUNCTION DESCRIPTION ********************************************
DoNormalComparison                  <MAIN_PARSER.LIB>

SYNTAX: int DoNormalComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoNormalComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal )
{
  	if ( pLVal->cVarType == VARTYPE_STRING ||
        pRVal->cVarType == VARTYPE_STRING )
   {
      // Promote both types to string if either type is a string before comparing.
      ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      if ( strcmp( g_oStringBuffer, g_oStringBuffer2 ) == 0 )
      	return TRUE;
      else
      	return FALSE;
   }
#if ENABLE_FLOAT
   else if ( pLVal->cVarType == VARTYPE_FLOAT ||
        		 pRVal->cVarType == VARTYPE_FLOAT )
   {
   	// Promote both types to float
   	if ( ConvertVariableToFloat( pLVal ) == ConvertVariableToFloat( pRVal ) )
      	return TRUE;
      else
      	return FALSE;
   }
#endif
   else if ( pLVal->cVarType == VARTYPE_INT ||
   			 pRVal->cVarType == VARTYPE_INT )
   {
   	// Promote both types to int
      if ( ConvertVariableToInt( pLVal ) == ConvertVariableToInt( pRVal ) )
      	return TRUE;
      else
      	return FALSE;
   }
   else if ( pLVal->cVarType == VARTYPE_BOOLEAN ||
   			 pRVal->cVarType == VARTYPE_BOOLEAN )
	{
      // Promote both types to int
      if ( ConvertVariableToBoolean( pLVal ) == ConvertVariableToBoolean( pRVal ) )
      	return TRUE;
      else
      	return FALSE;
   }
   else
   {
   	return FALSE;
   }
}



/* START FUNCTION DESCRIPTION ********************************************
DoStrictComparison                  <MAIN_PARSER.LIB>

SYNTAX: int DoStrictComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug int DoStrictComparison( HttpServerStruct *pServer, PHPVariable *pLVal, PHPVariable *pRVal )
{
   if ( pLVal->cVarType != pRVal->cVarType )
	{
   	return FALSE;
   }
   else if ( pLVal->cVarType == VARTYPE_STRING )
   {
      // Promote both types to string if either type is a string before comparing.
      ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      if ( strcmp( g_oStringBuffer, g_oStringBuffer2 ) == 0 )
      	return TRUE;
      else
      	return FALSE;
   }
#if 0
   else if ( pLVal->cVarType == VARTYPE_FLOAT )
   {
   	if ( pLVal->VarData.fFloatValue == pRVal->VarData.fFloatValue )
      	return TRUE;
      else
      	return FALSE;
   }
#endif
   else if ( pLVal->cVarType == VARTYPE_INT ||
   			 pLVal->cVarType == VARTYPE_BOOLEAN )
   {
      if ( pLVal->VarData.iIntValue == pRVal->VarData.iIntValue )
      	return TRUE;
      else
      	return FALSE;
   }
   else
   {
   	return FALSE;
   }
}


/* START FUNCTION DESCRIPTION ********************************************
DoLessThanComparison                  <MAIN_PARSER.LIB>

SYNTAX: void DoLessThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug void DoLessThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )
{
	pResult->cVarType = VARTYPE_BOOLEAN;

	if ( pLVal->cVarType == VARTYPE_STRING ||
        pRVal->cVarType == VARTYPE_STRING )
   {
      // Promote both types to string if either type is a string before comparing.
      ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      if ( strcmp( g_oStringBuffer, g_oStringBuffer2 ) < 0 )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
#if ENABLE_FLOAT
   else if ( pLVal->cVarType == VARTYPE_FLOAT ||
        		 pRVal->cVarType == VARTYPE_FLOAT )
   {
   	// Promote both types to float
   	if ( ConvertVariableToFloat( pLVal ) < ConvertVariableToFloat( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
#endif
   else if ( pLVal->cVarType == VARTYPE_INT ||
   			 pRVal->cVarType == VARTYPE_INT )
   {
   	// Promote both types to int
      if ( ConvertVariableToInt( pLVal ) < ConvertVariableToInt( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
   else if ( pLVal->cVarType == VARTYPE_BOOLEAN ||
   			 pRVal->cVarType == VARTYPE_BOOLEAN )
	{
      // Promote both types to int
      if ( ConvertVariableToBoolean( pLVal ) < ConvertVariableToBoolean( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
   else
   {
   	pResult->VarData.iIntValue = FALSE;
   }
}



/* START FUNCTION DESCRIPTION ********************************************
DoGreaterThanComparison                  <MAIN_PARSER.LIB>

SYNTAX: void DoGreaterThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug void DoGreaterThanComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )
{
	pResult->cVarType = VARTYPE_BOOLEAN;

	if ( pLVal->cVarType == VARTYPE_STRING ||
        pRVal->cVarType == VARTYPE_STRING )
   {
      // Promote both types to string if either type is a string before comparing.
      ConvertVariableToString( pLVal, g_oStringBuffer );
      ConvertVariableToString( pRVal, g_oStringBuffer2 );

      if ( strcmp( g_oStringBuffer, g_oStringBuffer2 ) > 0 )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
#if ENABLE_FLOAT
   else if ( pLVal->cVarType == VARTYPE_FLOAT ||
        		 pRVal->cVarType == VARTYPE_FLOAT )
   {
   	// Promote both types to float
   	if ( ConvertVariableToFloat( pLVal ) > ConvertVariableToFloat( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
#endif
   else if ( pLVal->cVarType == VARTYPE_INT ||
   			 pRVal->cVarType == VARTYPE_INT )
   {
   	// Promote both types to int
      if ( ConvertVariableToInt( pLVal ) > ConvertVariableToInt( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
   else if ( pLVal->cVarType == VARTYPE_BOOLEAN ||
   			 pRVal->cVarType == VARTYPE_BOOLEAN )
	{
      // Promote both types to int
      if ( ConvertVariableToBoolean( pLVal ) > ConvertVariableToBoolean( pRVal ) )
      	pResult->VarData.iIntValue = TRUE;
      else
      	pResult->VarData.iIntValue = FALSE;
   }
   else
   {
   	pResult->VarData.iIntValue = FALSE;
   }
}




/* START FUNCTION DESCRIPTION ********************************************
DoXORComparison                  <MAIN_PARSER.LIB>

SYNTAX: void DoXORComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1

RETURN VALUE:  None

SEE ALSO:

END DESCRIPTION **********************************************************/
_mparser_nodebug void DoXORComparison( HttpServerStruct *pServer, PHPVariable *pResult, PHPVariable *pLVal, PHPVariable *pRVal )
{
   auto int cLVal, cRVal;

   cLVal = ConvertVariableToBoolean( pLVal );
   cRVal = ConvertVariableToBoolean( pRVal );

   // (a || b) && (!(a && b))
   pResult->VarData.iIntValue = (cLVal || cRVal) && (!(cLVal && cRVal));
   pResult->cVarType = VARTYPE_BOOLEAN;
}
