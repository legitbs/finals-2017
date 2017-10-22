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
#include "tokenizer.h"

#ifndef EOF
#define EOF 	(-1)
#endif

/******************************************************************************
Name:			sTable
Description:	struct to store list of token id
******************************************************************************/
const SymbolTable sTable[] =
{
    {"!=="              , TOKEN_NOT_EQUAL_IDENT     , 3     },
    {"!="               , TOKEN_NOT_EQUAL           , 2     },
    {"!"                , TOKEN_NOT                 , 1     },
    {"%="               , TOKEN_MOD_EQUAL           , 2     },
    {"%"                , TOKEN_MOD                 , 1     },
    {"&="               , TOKEN_AMPERSAND_EQUAL     , 2     },
    {"&&"               , TOKEN_LOGICAL_AND         , 2     },
    {"&"                , TOKEN_AMPERSAND           , 1     },
    {"(boolean)"        , TOKEN_CAST_BOOLEAN        , 9     },
    {"(string)"         , TOKEN_CAST_STRING         , 8     },
    {"(char)"           , TOKEN_CAST_CHAR           , 6     },
    {"(int)"            , TOKEN_CAST_INT            , 5     },
    {"("                , TOKEN_OPEN_PAREN          , 1     },
    {")"                , TOKEN_CLOSE_PAREN         , 1     },
    {"*/"               , TOKEN_CLOSE_COMMENT       , 2     },
    {"*="               , TOKEN_STAR_EQUAL          , 2     },
    {"*"                , TOKEN_STAR                , 1     },
    {"++"               , TOKEN_INCREMENT           , 2     },
    {"+="               , TOKEN_ADD_EQUAL           , 2     },
    {"+"                , TOKEN_ADD                 , 1     },
    {","                , TOKEN_COMMA               , 1     },
    {"--"               , TOKEN_DECREMENT           , 2     },
    {"-="               , TOKEN_SUBTRACT_EQUAL      , 2     },
    {"-"                , TOKEN_SUBTRACT            , 1     },
    {".="               , TOKEN_PERIOD_EQUAL        , 2     },
    {"."                , TOKEN_PERIOD              , 1     },
    {"/*"               , TOKEN_OPEN_COMMENT        , 2     },
    {"//"               , TOKEN_DOUBLE_SLASH        , 2     },
    {"/="               , TOKEN_FORWARD_SLASH_EQUAL , 2     },
    {"/"                , TOKEN_FORWARD_SLASH       , 1     },
    {":"                , TOKEN_COLON               , 1     },
    {";"                , TOKEN_SEMICOLON           , 1     },
    {"<<="              , TOKEN_SHIFT_LEFT_EQUAL    , 3     },
    {"<=="              , TOKEN_LESS_EQUAL_IDENT    , 3     },
    {"<<"               , TOKEN_SHIFT_LEFT          , 2     },
    {"<="               , TOKEN_LESS_EQUAL          , 2     },
    {"<"                , TOKEN_LESS_THAN           , 1     },
    {"==="              , TOKEN_EQUAL_IDENT         , 3     },
    {"=="               , TOKEN_EQUALTO             , 2     },
    {"="                , TOKEN_ASSIGNMENT          , 1     },
    {">>="              , TOKEN_SHIFT_RIGHT_EQUAL   , 3     },
    {">>"               , TOKEN_SHIFT_RIGHT         , 2     },
    {">="               , TOKEN_GREATER_EQUAL       , 2     },
    {">"                , TOKEN_GREATER_THAN        , 1     },
    {"?>"               , TOKEN_PHP_CLOSE_TAG       , 2     },
    {"?"                , TOKEN_QUESTION_MARK       , 1     },
    {"["                , TOKEN_OPEN_BRACKET        , 1     },
    {"]"                , TOKEN_CLOSE_BRACKET       , 1     },
    {"^="               , TOKEN_XOR_EQUAL           , 2     },
    {"^"                , TOKEN_XOR                 , 1     },
    {"and"              , TOKEN_LOGICAL_AND2        , 3     },
    {"boolean"          , TOKEN_BOOLEAN             , 7     },
    {"break"            , TOKEN_BREAK               , 5     },
    {"continue"         , TOKEN_CONTINUE            , 8     },
    {"case"             , TOKEN_CASE                , 4     },
    {"char"             , TOKEN_CHAR                , 4     },
    {"default"          , TOKEN_DEFAULT             , 7     },
    {"do"               , TOKEN_DO                  , 2     },
    {"elseif"           , TOKEN_ELSEIF              , 6     },
    {"echo"             , TOKEN_ECHO                , 4     },
    {"else"             , TOKEN_ELSE                , 4     },
    {"function"         , TOKEN_FUNCTION            , 8     },
    {"foreach"          , TOKEN_FOR_EACH            , 7     },
    {"for"              , TOKEN_FOR                 , 3     },
    {"global"           , TOKEN_GLOBAL              , 6     },
    {"int"              , TOKEN_INT                 , 3     },
    {"if"               , TOKEN_IF                  , 2     },
    {"not"              , TOKEN_NOT                 , 3     },
    {"or"               , TOKEN_LOGICAL_OR2         , 2     },
    {"print"            , TOKEN_ECHO                , 5     },
    {"return"           , TOKEN_RETURN              , 6     },
    {"static"           , TOKEN_STATIC              , 6     },
    {"string"           , TOKEN_STRING              , 6     },
    {"switch"           , TOKEN_SWITCH              , 6     },
    {"while"            , TOKEN_WHILE               , 5     },
    {"xor"              , TOKEN_LOGICAL_XOR2        , 3     },
    {"{"                , TOKEN_OPEN_SQUIGGLY       , 1     },
    {"|="               , TOKEN_OR_EQUAL            , 2     },
    {"||"               , TOKEN_LOGICAL_OR          , 2     },
    {"}"                , TOKEN_CLOSE_SQUIGGLY      , 1     },
    {"~="               , TOKEN_TILDA_EQUAL         , 2     },
    {"~"                , TOKEN_TILDA               , 1     },

    {""                 , LIST_EOF                  , 10    }
};
/******************************************************************************
DEFINING GLOBAL VARIABLES
******************************************************************************/
unsigned int ucLocOpenParen;
unsigned int ucSymTableLen;
unsigned int ucLocComma;
unsigned int ucLocSemi;
unsigned int ucLocEqual;
unsigned int ucLocA;
unsigned int ucLocD;
unsigned int ucLocI;
unsigned int ucLocP;
unsigned int ucLocSquiggly;



/*** EndHeader */

/******************************************************************************
Name:			Tokenizer_Init
Description:	This function will gather data from the symbol table
Param:			n/a
Return:			TRUE OR FALSE
******************************************************************************/
_token_nodebug
int Tokenizer_Init()
{
	auto int i;
   ucLocOpenParen = 0;
   ucLocComma	=0;
	ucLocSemi	=0;
	ucLocEqual	=0;
	ucLocA		=0;
	ucLocD		=0;
	ucLocI		=0;
	ucLocP		=0;
	ucLocSquiggly	=0;

	for( i = 0; i< 250; i++)
	{
		switch( sTable[i].symbolString[0] )
		{
		case '(':
			if (!ucLocOpenParen)
				ucLocOpenParen = i;
			break;
		case ',':
			if (!ucLocComma)
				ucLocComma = i;
			break;
		case ';':
			if ( !ucLocSemi )
				ucLocSemi = i;
			break;
		case '=':
			if ( !ucLocEqual )
				ucLocEqual = i;
			break;
		case 'a':
			if ( !ucLocA )
				ucLocA = i;
			break;
		case 'd':
			if ( !ucLocD )
				ucLocD = i;
			break;
		case 'i':
			if (!ucLocI)
				ucLocI = i;
			break;
		case 'p':
			if (!ucLocP)
				ucLocP = i;
			break;
		case '{':
			if (!ucLocSquiggly)
				ucLocSquiggly = i;
			break;
		default:
			break;
		}
		if (sTable[i].ucSymbolValue == LIST_EOF )
		{
			ucSymTableLen = i+1;
			return TRUE;
		}
	}
	return FALSE;

}

/******************************************************************************
Name:			GotoNonSpace
Description:	Move Char ptr to first non space char. Ignoring space, tab
				and carage return.  This code will also skip all comments
				block.
******************************************************************************/
_token_nodebug
int GotoFirstNonSpace( HttpServerStruct *pServer )
{
   auto char *ppEndOfFile;
   auto PHPInterpreterStruct *pInterp;

   pInterp = &pServer->php_interpreter;
   ppEndOfFile = (pInterp->ppFileStartLocation + pInterp->iFileLength);

   while( pInterp->ppFileLocation < ppEndOfFile )	/* Check for End of File */
	{
   	switch( *(pInterp->pFileData) )
      {
      	case ' ':
         case '\t':
         case '\r':
         case '\n':
         	break;

      	default:
         	return TRUE;
      }


      // Increment the offset within the buffer
      pInterp->ppFileLocation++;
      pInterp->pFileData++;

   }

   return FALSE;

   /*
   while ( **pos == ' ' || **pos == '\t' ||
			 **pos == '\r' || **pos == '\n' )
	{
		if ( **pos == EOF )
			return FALSE;

		++*pos;
	}
	return TRUE;
   */
}

/******************************************************************************
Name:			ConvertStringToNum
Description:	Convert String into float
Param:			cStartString: location of string

output:			-1 Error code
				1  int found
******************************************************************************/
_token_nodebug
int ConvertStringToNum( int * lNum, HttpServerStruct *pServer )
{
	auto int isPeriod;
	auto unsigned int ucAfterPeriod;
   auto char *ppEndOfFile;
   auto PHPInterpreterStruct *pInterp;

	isPeriod = FALSE ;
	ucAfterPeriod = 0;

   pInterp = &pServer->php_interpreter;
   ppEndOfFile = (pInterp->ppFileStartLocation + pInterp->iFileLength);

   while( pInterp->ppFileLocation < ppEndOfFile )	/* Check for End of File */
   {
   	switch( *(pInterp->pFileData) )
      {
      	case '.':
				// Float not supported in this architecture
				return -1;				// Return Error code
            break;

         case '0':
         case '1':
         case '2':
      	case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
				*lNum = *lNum * 10 + ( *(pInterp->pFileData) - '0' );
         	break;

      	default:
				return 1;		// Int found
				break;
      }


      // Increment the offset within the buffer
      pInterp->ppFileLocation++;
      pInterp->pFileData++;

   }

   return -1;		// Return error code - end of file.
}


/******************************************************************************
Name:			isEndingTokenChar
Description:	This function check if the current position is an ending token
******************************************************************************/
_token_nodebug
int IsEndingTokenChar(int checkChar)
{
	if( checkChar >= '0' && checkChar <= '9' ||
		checkChar >= 'a' && checkChar <= 'z' ||
		checkChar >= 'A' && checkChar <= 'Z' ||
		checkChar == '_' )
	{
#if HTTP_VERBOSE
		printf( "Not ending character: %c\n", checkChar );
#endif
		return FALSE;
	}
	else
	{
#if HTTP_VERBOSE
		printf( "Yes ending character: %c\n", checkChar );
#endif
		return TRUE;
	}
}

/******************************************************************************
Name:			GotoEndingTokenChar
Description:	This function go to position of an ending token
******************************************************************************/
_token_nodebug
int GotoEndingTokenChar( HttpServerStruct *pServer )
{
   char *ppEndOfFile;
   PHPInterpreterStruct *pInterp;

   pInterp = &(pServer->php_interpreter);
   ppEndOfFile = (pInterp->ppFileStartLocation + pInterp->iFileLength);

   while( pInterp->ppFileLocation < ppEndOfFile )	/* Check for End of File */
   {
      int curChar = *(pInterp->pFileData);

#if HTTP_VERBOSE
	printf( "ENDING TOKEN CHECK: %c (%d) %d\n", curChar, (int)*(pInterp->pFileData), IsEndingTokenChar( curChar ) );
#endif

      if ( IsEndingTokenChar( curChar ) )
      	return TRUE;


      // Increment the offset within the buffer
      pInterp->ppFileLocation++;
      pInterp->pFileData++;

   }

   return FALSE;
}

/******************************************************************************
Name:			GotoFirstOccurChar
Description:	This function go to the first occurence of a specified char
******************************************************************************/
_token_nodebug
int GotoFirstOccurChar( HttpServerStruct *pServer, int char1)
{
   auto char *ppEndOfFile;
   auto PHPInterpreterStruct *pInterp;

   pInterp = &pServer->php_interpreter;
   ppEndOfFile = (pInterp->ppFileStartLocation + pInterp->iFileLength);

   while( pInterp->ppFileLocation < ppEndOfFile )	/* Check for End of File */
	{
      if ( *(pInterp->pFileData) == char1 )
      	return TRUE;


      // Increment the offset within the buffer
      pInterp->ppFileLocation++;
      pInterp->pFileData++;

   }

   return FALSE;
}

/******************************************************************************
Name:			CompareTokens
Description:	This function will compare two given tokens
param:			tokenPos - location of token on disk
				symbolString - token string to compare with
				symbolStringLen - str length of symbolString
output:	   A nonzero value indicating the length of the token, otherwise
				-1 if not found.
******************************************************************************/
_token_nodebug
int CompareTokens( char *pSymbol, unsigned int ucStart,
				    unsigned int ucEnd, TokenStruct *T)
{
	auto unsigned int i;
	auto int found;
	auto char *tokenPos;

	found = FALSE;
	for ( i = ucStart; i <= ucEnd; i ++ )
	{
		if ( strncmp( sTable[i].symbolString, pSymbol,
			 sTable[i].ucSymbolStringLen ) == 0 )
		{
			found = TRUE;
			break;
		}
	}

	if (found == FALSE)
		return -1;

	tokenPos = pSymbol + sTable[i].ucSymbolStringLen;

   if( IsEndingTokenChar( *(tokenPos - 1) ) || IsEndingTokenChar( *tokenPos ) )
	{
		T->ucTokenType = TYPE_SYMBOL;
		T->TokenData.ucSymbolValue = sTable[i].ucSymbolValue;

      return sTable[i].ucSymbolStringLen;
	}
	else
		return -1;
}

/******************************************************************************
Name:			BinSearchSymbol
Description:	This function will perform a binary search for a symbol
Param:			cppCurrentPos
				pToken
Return:			TRUE OR FALSE
******************************************************************************/
_token_nodebug
int BinSearchSymbol( TokenStruct * T, HttpServerStruct *pServer )
{
	auto int found;
   auto char *pFileData;

   pFileData = pServer->php_interpreter.pFileData;

 	if( *pFileData >= '{' )
	{
		found = CompareTokens( pFileData, ucLocSquiggly , ucSymTableLen, T );
	}
	else if( *pFileData >= 'p' )
	{
		found = CompareTokens( pFileData, ucLocP , ucLocSquiggly, T );
	}
	else if( *pFileData >= 'i' )
	{
		found = CompareTokens( pFileData, ucLocI , ucLocP, T );
	}
	else if( *pFileData >= 'd' )
	{
		found = CompareTokens( pFileData, ucLocD , ucLocI, T );
	}
	else if( *pFileData >= 'a' )
	{
		found = CompareTokens( pFileData, ucLocA , ucLocD, T );
	}
	else if( *pFileData >= '=' )
	{
		found = CompareTokens( pFileData, ucLocEqual , ucLocA, T );
	}
	else if( *pFileData >= ';' )
	{
		found = CompareTokens( pFileData, ucLocSemi , ucLocEqual, T );
	}
	else if ( *pFileData >= ',' )
	{
		found = CompareTokens( pFileData, ucLocComma , ucLocSemi, T );
	}
   else if ( *pFileData >= '(' )
	{
		found = CompareTokens( pFileData, ucLocOpenParen , ucLocComma, T );
	}
	else
	{
		found = CompareTokens( pFileData, 0 , ucLocComma, T );
	}

	if ( found == -1 )
	{
      T->ucTokenType = LIST_EOF;
		return FALSE;
	}
	else
	{
   	pServer->php_interpreter.pFileData += found;
      pServer->php_interpreter.ppFileLocation += found;

      return TRUE;
	}
}

/******************************************************************************
Name:			GetCurrentToken
Description:	This function will return the current token
******************************************************************************/
_token_nodebug
int GetCurrentToken ( TokenStruct *T, HttpServerStruct *pServer )
{
	auto int lNum;
	auto char *ppSaveCurrentPos;
	auto char *cpTemp;
   auto int cTemp;
   auto unsigned int ucCounter;
   auto PHPInterpreterStruct *pInterp;
   auto char *ppEndOfFile;

	lNum = 0;

   // GotoFirstNonSpace will center the window where we want it to be.
   GotoFirstNonSpace( pServer );

   pInterp = (&pServer->php_interpreter);

   // BUG:: This should be ppFileStartLocation! (original code -- left it in)
   ppEndOfFile = pInterp->ppFileLocation + pInterp->iFileLength;

   ppSaveCurrentPos = pInterp->ppFileLocation;
   T->ppTokenLocation = pInterp->ppFileLocation;


#if HTTP_VERBOSE
   printf( "GetCurrentToken! %c\n", *(pInterp->pFileData) );
#endif

	/* Checking if token is a constant number */
	if( *(pInterp->pFileData) >= '0' && *(pInterp->pFileData) <= '9' )
	{
		switch ( ConvertStringToNum( &lNum, pServer ) )
		{
		case -1:
			return -1;
			break;
		case 1:
			T->ucTokenType = TYPE_LONG;
			T->TokenData.lValue = lNum;
			break;
		}
	}
	/* check token for type string */
	else if ( *(pInterp->pFileData) == '"' )
	{
		pInterp->pFileData++;
		pInterp->ppFileLocation++;

      cTemp = '\0'; /* Initializing cTemp */

      /* Read in a string */
		for ( ucCounter = 0; ucCounter < PHP_MAX_STRINGLENGTH && pInterp->ppFileLocation < ppEndOfFile; ucCounter++ )
		{
         switch( *(pInterp->pFileData) )
         {
          	case EOF:
            	T->ucTokenType = LIST_EOF;
					return FALSE;

            case '\\':
            	pInterp->pFileData++;
               pInterp->ppFileLocation++;

               switch( *(pInterp->pFileData) )
               {
               case 'n':
               	cTemp = '\n';
                  break;

               case 't':
               	cTemp = '\t';
                  break;

               case 'r':
               	cTemp = '\r';
                  break;

               case '0':
               	cTemp = '\0';
                  break;

               case '\\':
               	cTemp = '\\';
                  break;

               case '"':
               	cTemp = '"';
                  break;

               }
               break;

            case '"':
               // Skip over the closing quote
               pInterp->pFileData++;
               pInterp->ppFileLocation++;

               T->sStringBuffer[ucCounter] = '\0';
	            T->ucTokenType = TYPE_STRING;
	            T->TokenData.ucStringLen = ucCounter + 1;

#if HTTP_VERBOSE
					printf( "TOKEN: ::STRING (%s) (len=%d)!\n", T->sStringBuffer, ucCounter+1 );
#endif

	            return TRUE;

            default:
            	cTemp = *(pInterp->pFileData);

               // ++ *currentPos;
               pInterp->pFileData++;
               pInterp->ppFileLocation++;

               break;
         }
         T->sStringBuffer[ucCounter] = cTemp;
  		}

      /* String has exceed max size char, return false */
      T->ucTokenType = LIST_EOF;
      return FALSE;
	}
	/* Check for Char Type */
	else if ( *(pInterp->pFileData) == '\'' ) /* Token is a char type */
	{
		if( *(pInterp->pFileData + 2) == '\'')
		{
			T->ucTokenType = TYPE_CHAR;
			T->TokenData.cCharValue = *(pInterp->pFileData + 1);

         // Move past three characters
         pInterp->pFileData += 3;
         pInterp->ppFileLocation += 3;

         //*currentPos = *currentPos + 3;
		}
		else
		{
			/* Searching for the closing quotation */
         // ++*currentPos;
         pInterp->pFileData++;
         pInterp->ppFileLocation++;

         cTemp = '\0'; /* Initializing cTemp */

			/* Searching for the closing quotation */
			for ( ucCounter = 0; ucCounter < PHP_MAX_STRINGLENGTH && pInterp->ppFileLocation < ppEndOfFile; ucCounter ++ )
			{
         	switch( *(pInterp->pFileData) )
         	{
          		case EOF:
            		T->ucTokenType = LIST_EOF;
						return FALSE;

               case '\'':
            		// ++ *currentPos; /* Skipping over the closing quote */
                  pInterp->pFileData++;
                  pInterp->ppFileLocation++;

                  T->sStringBuffer[ucCounter] = '\0';
	            	T->ucTokenType = TYPE_STRING;
	            	T->TokenData.ucStringLen = ucCounter +1;
	            	return TRUE;

               default:
            		cTemp = *(pInterp->pFileData);

                  // ++ *currentPos;
                  pInterp->pFileData++;
                  pInterp->ppFileLocation++;
                  break;
         	}
         	T->sStringBuffer[ucCounter] = cTemp;
  			}

         /* String has exceed max size char, return false */
      	T->ucTokenType = LIST_EOF;
      	return FALSE;
		}

	}
	else if ( *(pInterp->pFileData) == '$' ) /* Search for variable type */
	{
		// ++*currentPos;				/* Skipping over $ */
      pInterp->pFileData++;
      pInterp->ppFileLocation++;

		if ( GotoEndingTokenChar( pServer ) )
		{
			/* get length of var */
			T->TokenData.ucIdentifierLen = (pInterp->ppFileLocation - ppSaveCurrentPos - 1);

         if( T->TokenData.ucIdentifierLen <= 0 )
			{
				T->ucTokenType = LIST_EOF;
				return FALSE;
			}
			else
			{
				/* Variable found */
				T->ucTokenType = TYPE_VARIABLE;
				return TRUE;
			}
		}
		else
		{
			T->ucTokenType = LIST_EOF;
			return FALSE;
		}
	}

   /* Perform binary search for a current symbol */
	else if ( BinSearchSymbol( T, pServer ) )
	{
		/* check for closing PHP tag */
		if ( T->TokenData.ucSymbolValue == TOKEN_PHP_CLOSE_TAG )
		{
			http_flush_inout_buffer( pServer );
			/*
			if ( PHPTextParser( pServer ) == FALSE )
         {
         	// End of file found - return LIST_EOF
            T->ucTokenType = LIST_EOF;
            return FALSE;
         }
         else
				return GetCurrentToken(T, pServer);
			*/
			T->ucTokenType = LIST_EOF;
			return FALSE;
		}
		else if( T->TokenData.ucSymbolValue == TOKEN_DOUBLE_SLASH )
		{
			/* Skipping Comment block */
         for (;;)
         {
         	if ( pInterp->ppFileLocation >= ppEndOfFile ||
            	  *(pInterp->pFileData) == EOF )
            {
            	T->ucTokenType = LIST_EOF;
               return FALSE;
            }

         	if ( *(pInterp->pFileData) == '\n' ||
            	  *(pInterp->pFileData) == '\r' )
                 break;

            pInterp->pFileData++;
            pInterp->ppFileLocation++;
         }
			/*
         while ( **currentPos != '\r' && **currentPos != '\n' )
			{
				++*currentPos;
				if ( **currentPos == EOF )
				{
            	T->ucTokenType = LIST_EOF;
					return FALSE;
            }
			}
         */
			// ++*currentPos;
         pInterp->pFileData++;
         pInterp->ppFileLocation++;

			return GetCurrentToken( T, pServer );

		}
		else if ( T->TokenData.ucSymbolValue == TOKEN_OPEN_COMMENT )
		{
         for (;;)
         {
         	if ( pInterp->ppFileLocation >= ppEndOfFile ||
            	  *(pInterp->pFileData) == EOF )
            {
            	T->ucTokenType = LIST_EOF;
               return FALSE;
            }

         	if ( *(pInterp->pFileData) == '*' &&
            	  *(pInterp->pFileData+1) == '/' )
                 break;

            pInterp->pFileData++;
            pInterp->ppFileLocation++;

         }

         pInterp->pFileData+=2;
         pInterp->ppFileLocation+=2;

			// cpTemp = strstr(*currentPos, "*/");
			/*
         if ( cpTemp != NULL )
			{
				*currentPos = cpTemp + 2;
				return GetCurrentToken( T, pServer );
			}
			else
			{
				T->ucTokenType = LIST_EOF;
				return FALSE;
			}
         */
		}
		else
		{
			return TRUE;
		}
	}
	/* checking if token is a veriable name */
	else if (	*(pInterp->pFileData) >= 'A' && *(pInterp->pFileData) <= 'Z' ||
					*(pInterp->pFileData) >= 'a' && *(pInterp->pFileData) <= 'z' )
	{
#if HTTP_VERBOSE
		printf( "GetCurrentToken::Parsing identifier!\n" );
#endif

		GotoEndingTokenChar( pServer );
		T->ucTokenType = TYPE_IDENTIFIER;
		T->TokenData.ucIdentifierLen = (unsigned int)(pInterp->ppFileLocation - ppSaveCurrentPos);

#if HTTP_VERBOSE
		printf( "IDENTIFIER: (%s) %u\n", T->ppTokenLocation, T->TokenData.ucIdentifierLen );
#endif
	}
	else
	{
		T->ucTokenType = LIST_EOF;
		return FALSE; /* No token found */
	}
	return TRUE;
}

/******************************************************************************
Name:			PHPTextParser
Description:	This function will seperate HTML code form PHP code
Param:			cppFileLocation - points to file
				cppFileLocation will point to next <?PHP token
Return:			char value representing error code.
******************************************************************************/
_token_nodebug
int PHPTextParser( HttpServerStruct *pServer )
{
  	auto unsigned int iLength;
   auto int cState;
   auto char *ppEndOfFile;
   auto char *ppStartLocation;
   auto char *pFileData;
   auto PHPInterpreterStruct *pInterp;

#if HTTP_VERBOSE
   printf( "PHPTextParser::Called!\n" );
#endif

  	cState = 0;
   pInterp = &(pServer->php_interpreter);

   ppEndOfFile = (pInterp->ppFileStartLocation + pInterp->iFileLength);
   ppStartLocation = pInterp->ppFileLocation;
   pFileData = pInterp->pFileData;

   while( pInterp->ppFileLocation < ppEndOfFile )	/* Check for End of File */
	{
     	// Scan for a PHP code block
     	switch( *pFileData )
      {
         case '<':
            if ( cState == 0 )
               cState = 1;
            else
               cState = 0;

            break;

         case '?':
            if ( cState == 1 )
               cState = 2;
            else
               cState = 0;
            break;

         case 'p':
         case 'P':
            if ( cState == 2 )
               cState = 3;
            else if ( cState == 4 )
               cState = 5;
            else
               cState = 0;

            break;

         case 'h':
         case 'H':
            if ( cState == 3 )
               cState = 4;
            else
               cState = 0;
            break;

         default:
            cState = 0;
      }

      // Increment the files location and the file data in the window.
      pInterp->ppFileLocation++;
      pFileData++;

      if ( cState == 5 )
      	break;
   }

   // Reset the sliding window
   MoveSlidingWindow( pServer, TRUE );

   // Reuse end of file to indicate end of the stream
   ppEndOfFile = (pInterp->ppFileLocation - cState);

   // Write out the html code in blocks that fit into the inout_buffer
   /*
   while( pStartLocation < pEndOfFile )
   {
   	if ( (pStartLocation + HTTP_MAX_INOUTBUFFER) > pEndOfFile )
      	iLength = (pEndOfFile - pStartLocation);
      else
      	iLength = HTTP_MAX_INOUTBUFFER;

      // Copy the html code to the output buffer
      http_write_output( pServer, pStartLocation, iLength );

      // Move past the chunk we have written
      pStartLocation += iLength;
   }
   */
   if ( pServer->php_interpreter.bIgnoreHTMLCode == FALSE )
   {
   	// Force a flush of the inout_buffer since we will be modifying the xmem area directly
   	http_flush_inout_buffer( pServer );

   	// Get the length to copy.
   	iLength = (unsigned int)(ppEndOfFile - ppStartLocation);

   	// Copy the data into the output buffer for writing.
		http_write_output( pServer, ppStartLocation, iLength );
   	//memcpy( (pServer->output_buffer_reserved_loc + pServer->output_length), ppStartLocation, iLength );
   	//pServer->output_length += iLength;
   }

   if ( cState == 5 )
   	return TRUE;
   else
   	return FALSE; 	// We hit end of file prematurely.
}

/******************************************************************************
NOTE: THIS IS TEST CODE
TODO: REMOVE THIS CODE WHEN TESTING IS DONE
******************************************************************************/
_token_nodebug
int TestInterpreter( HttpServerStruct *pServer )
{
	auto int i;
   auto TokenStruct T1;
   auto int len;

   if ( PHPTextParser( pServer ) == FALSE )
   {
   	return TRUE;
   }

   while ( GetCurrentToken( &T1, pServer ) == TRUE )
	{
   	len = 0;
		switch ( T1.ucTokenType )
		{
		case TYPE_SYMBOL:
			len += sprintf( g_oStringBuffer, "\nTokenType: Symbol Value: %d<BR>\n", T1.TokenData.ucSymbolValue );
			break;
		case TYPE_LONG:
			len += sprintf( g_oStringBuffer, "\nTokenType: Long Value: %d<BR>\n", T1.TokenData.lValue );
			break;
#if 0
		case TYPE_FLOAT:
			len += sprintf( g_oStringBuffer, "\nTokenType: Float Value: %f<BR>\n", T1.TokenData.fValue );
			break;
#endif
		case TYPE_IDENTIFIER:
         len += sprintf( g_oStringBuffer, "\nTokenType: Ident Value: BLAH<BR>\n" );
			break;
		case TYPE_STRING:
         len += sprintf( g_oStringBuffer, "\nTokenType: String Value: %s<BR>\n", T1.sStringBuffer );
         break;
		case TYPE_VARIABLE:
         len += sprintf( g_oStringBuffer, "\nTokenType: variable value: %s <BR>\n", T1.sStringBuffer );
         break;
		}
      http_write_output( pServer, g_oStringBuffer, len );
	}
}
