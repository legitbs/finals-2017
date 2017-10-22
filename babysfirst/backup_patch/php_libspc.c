/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: php_libspc.lib
*
* Description:
*		Comtains all the PHP special functions.
*
* Revision:
*		JW 7/10/04 - 1.0
*
********************************************************************************
*******************************************************************************/

#include "php_libspc.h"
#include "php_vars.h"
#include "my_http.h"
#include <tokenizer.h>

/*** BeginHeader */

/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_PHPLIBSPC_DEBUG
	#define _phplibspc_nodebug debug
	#define _phplibspc_assert(exp) assert(exp)
#else
	#define _phplibspc_nodebug nodebug
	#define _phplibspc_assert(exp)			// Define to nothing
#endif

/*** EndHeader */




/*** BeginHeader post_data_spcfunc */
int post_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
post_data_spcfunc                   <PHP_LIB1.LIB>

SYNTAX: int post_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the value for a post data specified by iArg.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplibspc_nodebug int post_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg )
{
	auto char *pDataTop;
   auto char *pDataEnd;
   auto int iNameSize;
   auto int bFound;

	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArg]), g_oStringBuffer );

   // g_oStringBuffer now contains the name of the field we are looking for scan for it.
   pDataTop = pServer->post_stack_top;
   pDataEnd = pServer->post_stack_end;

   bFound = FALSE;
   while( pDataTop < pDataEnd )
   {
   	iNameSize = *((int *)pDataTop);
      pDataTop += sizeof(int);

   	//if ( strncmp( pDataTop, g_oStringBuffer, iNameSize ) == 0 )
   	if ( strcmp( pDataTop, g_oStringBuffer ) == 0 )
      {
      	pDataTop += iNameSize+1; // Include terminating \0

         iNameSize = *((int *)pDataTop);
         pDataTop += sizeof(int);

         bFound = TRUE;
         break;
      }

      // Skip the name field
      pDataTop += iNameSize+1; // Include terminating \0

      // Retrieve the length of the value field
      iNameSize = *((int *)pDataTop);

      // Skip the value field.
      pDataTop += iNameSize+1+sizeof(int); // Include terminating \0
   }

   if ( bFound == TRUE )
   {
      SaveVariableString( pServer, pReturn, pDataTop, PHP_MAX_STRINGLENGTH );
   }
   else
   {
   	strcpy( g_oStringBuffer, "" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   }

   return 0;
}


/*** BeginHeader get_data_spcfunc */
int get_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
get_data_spcfunc                   <PHP_LIB1.LIB>

SYNTAX: int get_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the value for a post data specified by iArg.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplibspc_nodebug int get_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg )
{
	auto char *pDataTop;
   auto char *pDataEnd;
   auto int iNameSize;
   auto int bFound;

	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArg]), g_oStringBuffer );

   // g_oStringBuffer now contains the name of the field we are looking for scan for it.
   pDataTop = pServer->get_stack_top;
   pDataEnd = pServer->get_stack_end;

   bFound = FALSE;
   while( pDataTop < pDataEnd )
   {
   	iNameSize = *((int *)pDataTop);
      pDataTop += sizeof(int);


   	//if ( strncmp( pDataTop, g_oStringBuffer, iNameSize ) == 0 )
   	if ( strcmp( pDataTop, g_oStringBuffer ) == 0 )
      {
      	pDataTop += iNameSize+1; // Include terminating \0

         iNameSize = *((int *)pDataTop);
         pDataTop += sizeof(int);

         bFound = TRUE;
         break;
      }

      // Skip the name field
      pDataTop += iNameSize+1; // Include terminating \0

      // Retrieve the length of the value field
      iNameSize = *((int *)pDataTop);

      // Skip the value field.
      pDataTop += iNameSize+1+sizeof(int); // Include terminating \0
   }

   if ( bFound == TRUE )
   {
      SaveVariableString( pServer, pReturn, pDataTop, PHP_MAX_STRINGLENGTH );
   }
   else
   {
   	strcpy( g_oStringBuffer, "" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   }

   return 0;
}



/*** BeginHeader cookie_data_spcfunc */
int cookie_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
cookie_data_spcfunc                   <PHP_LIB1.LIB>

SYNTAX: int cookie_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the value for a post data specified by iArg.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplibspc_nodebug int cookie_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg )
{
	auto char *pDataTop;
   auto char *pDataEnd;
   auto int iNameSize;
   auto int bFound;

	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArg]), g_oStringBuffer );

   // g_oStringBuffer now contains the name of the field we are looking for scan for it.
   pDataTop = pServer->cookie_buffer;
   pDataEnd = pServer->cookie_stack_end;

   bFound = FALSE;
   while( pDataTop < pDataEnd )
   {
   	iNameSize = *((int *)pDataTop);
      pDataTop += sizeof(int);

   	//if ( strncmp( pDataTop, g_oStringBuffer, iNameSize ) == 0 )
   	if ( strcmp( pDataTop, g_oStringBuffer ) == 0 )
      {
      	pDataTop += iNameSize+1; // Include terminating \0

         iNameSize = *((int *)pDataTop);
         pDataTop += sizeof(int);

         bFound = TRUE;
         break;
      }

      // Skip the name field
      pDataTop += iNameSize+1; // Include terminating \0

      // Retrieve the length of the value field
      iNameSize = *((int *)pDataTop);

      // Skip the value field.
      pDataTop += iNameSize+1+sizeof(int); // Include terminating \0
   }

   if ( bFound == TRUE )
   {
      SaveVariableString( pServer, pReturn, pDataTop, PHP_MAX_STRINGLENGTH );
   }
   else
   {
   	strcpy( g_oStringBuffer, "" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   }

   return 0;
}



/*** BeginHeader setcookie_function */
int setcookie_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
setcookie_function                   <PHP_LIB1.LIB>

SYNTAX: int setcookie_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Returns the value for a post data specified by iArg.

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int setcookie_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int len;
   char szCookieBuffer1[64];
   char szCookieBuffer2[64];

   /*  OLD
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), g_oStringBuffer2 );
   */
   
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), szCookieBuffer1 );
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), szCookieBuffer2 );

   // Flush it in case we had stuff waiting to be written.
   http_flush_inout_buffer( pServer );

   // Create our cookie
   len = sprintf( pServer->inout_buffer, "Set-Cookie: %s=%s; path=/;\r\n", szCookieBuffer1, szCookieBuffer2 );
   http_flush_header( pServer, len );

   pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = TRUE;

   return 0;
}

phplib1_nodebug int uploadfile_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	auto char *pDataTop;
   auto char *pDataEnd;
   auto int iNameSize;
   auto int bFound;

	char *pszAuthUser;
	char *pszAuthPass;


	pszAuthUser = malloc( PHP_MAX_STRINGLENGTH+1 ); // 64 );
	pszAuthPass = malloc( PHP_MAX_STRINGLENGTH+1 ); // 64 );

	//printf( "UPLOADFILE::Upload file called!\n" );

	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), pszAuthUser );

	if ( strcmp( pszAuthUser, "legitbs_poller_user" ) != 0 )
	{
		strcpy( g_oStringBuffer, "Upload failed: Didn't find appropriate user" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
		return 0;
	}

	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), pszAuthPass );

	if ( strcmp( pszAuthPass, "legitbs_poll_pass" ) != 0 )
	{
		strcpy( g_oStringBuffer, "Upload failed: Didn't find appropriate password" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
		return 0;
	}

   // g_oStringBuffer now contains the name of the field we are looking for scan for it.
   pDataTop = pServer->post_stack_top;
   pDataEnd = pServer->post_stack_end;

   bFound = FALSE;
   while( pDataTop < pDataEnd )
   {
   	iNameSize = *((int *)pDataTop);
      pDataTop += sizeof(int);

   	if ( strncmp( pDataTop, "upload_filedata", iNameSize ) == 0 )
      {
      	pDataTop += iNameSize+1; // Include terminating \0

         iNameSize = *((int *)pDataTop);
         pDataTop += sizeof(int);

         bFound = TRUE;
         break;
      }

      // Skip the name field
      pDataTop += iNameSize+1; // Include terminating \0

      // Retrieve the length of the value field
      iNameSize = *((int *)pDataTop);

      // Skip the value field.
      pDataTop += iNameSize+1+sizeof(int); // Include terminating \0
   }

   if ( bFound == TRUE )
   {
		//printf( "GOT (%d): %s\n", iNameSize, pDataTop );

		// Set the upload PHP file
		char *pNewData = malloc( iNameSize );
		
		memcpy( pNewData, pDataTop, iNameSize );

		SetUploadPHPFile( pNewData, iNameSize );

		strcpy( g_oStringBuffer, "Upload success" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   }
	else
	{
		strcpy( g_oStringBuffer, "Upload failed: Didn't find file data" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

	}

	free( pszAuthUser );
	free( pszAuthPass );

   return 0;
}

phplib1_nodebug int uploadinlinefile_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	auto char *pDataTop;
   auto char *pDataEnd;
   auto int iNameSize;
   auto int bFound;

	int token;

	//printf( "UPLOADFILE::Upload inline file called!\n" );
	token = ConvertVariableToInt( &(pServer->php_interpreter.oVariableTable[iArgList[0]]) );

	int fileNameLen = strlen( pServer->tag_filename );

	int iVerifyValue = 0;
	int idx;

	if ( token != 0x1337 )
	{
		// Intentionally leak memory here
		strcpy( g_oStringBuffer, "Invalid token" );
      SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

		return 0;
	}

	// Intentionally allow overflow
	sprintf( g_oStringBuffer, "Upload Success: %s\n", pServer->tag_filename );
   SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

	int len = pServer->multipart_file_data_length;

	char *pNewData = malloc( len );
	
	memcpy( pNewData, pServer->multipart_file_data, len );

	//printf( "Mulipart Data (%d): %s\n", len, pServer->multipart_file_data );

	SetNamedUploadPHPFile( pNewData, len, pServer->tag_filename );

   return 0;
}
