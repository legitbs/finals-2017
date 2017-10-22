/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: php_lib1.lib
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
#include "php_vars.h"
#include "tokenizer.h"
#include "out_helper.h"

#include <time.h>


/*** BeginHeader */


/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_PHPLIB1_DEBUG
	#define _phplib1_nodebug debug
	#define _phplib1_assert(exp) assert(exp)
#else
	#define _phplib1_nodebug nodebug
	#define _phplib1_assert(exp)			// Define to nothing
#endif


#define 				MAX_FLOAT 	 		9999999
#define 				MAX_INT	 	 		9999999
/*** EndHeader */


time_t g_globalTime;

/*** BeginHeader test_function */
int test_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
test_function                   <PHP_LIB1.LIB>

SYNTAX: int test_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int test_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[0]])) +
   									  ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[1]]));

   return 0;
}

/*** BeginHeader pow_function */
int pow_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
test_function                   <PHP_LIB1.LIB>

SYNTAX: int pow_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int pow_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   if (pServer->php_interpreter.oVariableTable[iArgList[0]].cVarType == VARTYPE_INT &&
       pServer->php_interpreter.oVariableTable[iArgList[1]].cVarType == VARTYPE_INT )
   {
   	pReturn->cVarType = VARTYPE_INT;
   	pReturn->VarData.iIntValue = (int)my_pow( ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[0]])),
   												ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[1]])) );
   }
   else
   {
#if 0
   	pReturn->cVarType = VARTYPE_FLOAT;
   	pReturn->VarData.fFloatValue = pow( ConvertVariableToFloat(&(pServer->php_interpreter.oVariableTable[iArgList[0]])),
   												ConvertVariableToFloat(&(pServer->php_interpreter.oVariableTable[iArgList[1]])) );
#endif
   }

   return 0;
}

/*** BeginHeader srand_function */
int srand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
srand_function                   <PHP_LIB1.LIB>

SYNTAX: int srand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int srand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int iNum;
   iNum = ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[0]])) ;
   pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = 0;
   if( iNum % 2 == 0 )
   {
    	srand( 1 );
   }
   else
   {
   	srand( iNum );
   }

   return 0;
}

/*** BeginHeader rand_function */
int rand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
rand_function                   <PHP_LIB1.LIB>

SYNTAX: int rand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int rand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = rand( );

   return 0;
}

/*** BeginHeader strlen_function */
int strlen_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strlen_function                   <PHP_LIB1.LIB>

SYNTAX: int strlen_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strlen_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );
	pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = strlen(g_oStringBuffer);

   return 0;
}

/*** BeginHeader strcmp_function */
int strcmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strcmp_function                   <PHP_LIB1.LIB>

SYNTAX: int strcmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strcmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), g_oStringBuffer2 );
	pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = strcmp(g_oStringBuffer , g_oStringBuffer2);

   return 0;
}


/*** BeginHeader strncmp_function */
int strncmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strncmp_function                   <PHP_LIB1.LIB>

SYNTAX: int strcmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strncmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	auto int iNum;

   iNum = ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[2]]));

   if( iNum < 0 )
   {
   	pReturn->cVarType = VARTYPE_INT;
   	pReturn->VarData.iIntValue = MAX_INT;
   }
   else
   {
	   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );
	   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), g_oStringBuffer2 );
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = strncmp(g_oStringBuffer , g_oStringBuffer2, iNum);
   }

   return 0;
}


/*** BeginHeader strtolower_function */
int strtolower_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strtolower_function                   <PHP_LIB1.LIB>

SYNTAX: int strtolower_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strtolower_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int i;
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      	break;
      g_oStringBuffer[i] = (char)tolower(g_oStringBuffer[i]);
   }
   SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

   return 0;
}


/*** BeginHeader strtoupper_function */
int strtoupper_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strtoupper_function                   <PHP_LIB1.LIB>

SYNTAX: int strtoupper_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strtoupper_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int i;
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      	break;
      g_oStringBuffer[i] = (char)toupper(g_oStringBuffer[i]);
   }
   SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   return 0;
}

/*** BeginHeader setdatetime_function */
int setdatetime_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
setdatetime_function                   <PHP_LIB1.LIB>

SYNTAX: int setdatetime_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int setdatetime_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int iNum[6];
   auto unsigned int	i;
   auto struct tm		rtc;					// time struct
   auto unsigned int	t0, t1, t2;			// used for date->seconds conversion

   // BUG:: Accept as many integers as they pass
   for ( i = 0; i < iArgCount; i ++)
   {
   	iNum[i] = ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[i]]));
   }

   if ( iArgCount != 6 )
   {
       return 0;
   }


	//////////////////////////////////////////////////
	// read current date/time via tm_rd

	//tm_rd(&rtc);						// get time in struct tm

	//////////////////////////////////////////////////
	// change the date/time via tm_wr

	/*
	rtc.tm_sec 	= iNum[0];							// change the time
	rtc.tm_min 	= iNum[1];
	rtc.tm_hour = iNum[2];

	rtc.tm_mday = iNum[3];
	rtc.tm_mon 	= iNum[4];
	rtc.tm_year = iNum[5];
	*/
	struct tm rtc;

	rtc.tm_sec = iNum[0];
	rtc.tm_min = iNum[1];
	rtc.tm_hour = iNum[2];

	rtc.tm_mday = iNum[3];
	rtc.tm_mon = iNum[4];
	rtc.tm_year = iNum[5];

	g_globalTime = mktime( rtc );

	// stime( curtime );

   pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = 0;
   return 0;
}

/*** BeginHeader getdate_function */
int getdate_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
getdate_function                   <PHP_LIB1.LIB>

SYNTAX: int getdate_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int getdate_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto char *wk, *mth;
   auto int time;
   auto struct tm t;

   // TODO: Fix time
   t = localtime( &g_globalTime );

   switch (t.tm_wday)
   {
   case 0: wk = "Sun"; break;
   case 1: wk = "Mon"; break;
   case 2: wk = "Tue"; break;
   case 3: wk = "Wed"; break;
   case 4: wk = "Thu"; break;
   case 5: wk = "Fri"; break;
   case 6: wk = "Sat"; break;
   }

   switch (t.tm_mon)
   {
   case 1: mth = "Jan"; break;
   case 2: mth = "Feb"; break;
   case 3: mth = "Mar"; break;
   case 4: mth = "Apr"; break;
   case 5: mth = "May"; break;
   case 6: mth = "Jun"; break;
   case 7: mth = "Jul"; break;
   case 8: mth = "Aug"; break;
   case 9: mth = "Sep"; break;
   case 10: mth = "Oct"; break;
   case 11: mth = "Nov"; break;
   case 12: mth = "Dec"; break;
   }

   sprintf(g_oStringBuffer, "%s, %d %s %d %d:%d:%d GMT",
           wk, t.tm_mday, mth, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);

   SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
   return 0;
}

/*** BeginHeader is_int_function */
int is_int_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
is_int_function                   <PHP_LIB1.LIB>

SYNTAX: int is_int_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int is_int_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   if ( pServer->php_interpreter.oVariableTable[iArgList[0]].cVarType == VARTYPE_INT )
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = TRUE;
   }
   else
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = FALSE;
   }


   return 0;
}

/*** BeginHeader is_float_function */
int is_float_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
is_float_function                   <PHP_LIB1.LIB>

SYNTAX: int is_float_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int is_float_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   if ( pServer->php_interpreter.oVariableTable[iArgList[0]].cVarType == VARTYPE_FLOAT )
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = TRUE;
   }
   else
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = FALSE;
   }


   return 0;
}

/*** BeginHeader is_string_function */
int is_string_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
is_string_function                   <PHP_LIB1.LIB>

SYNTAX: int is_string_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int is_string_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   if ( pServer->php_interpreter.oVariableTable[iArgList[0]].cVarType == VARTYPE_STRING )
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = TRUE;
   }
   else
   {
	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = FALSE;
   }


   return 0;
}

/*** BeginHeader trim_function */
int trim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
trim_function                   <PHP_LIB1.LIB>

SYNTAX: int trim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int trim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int i;
   auto int iStartLoc;
   auto char * cpStartString;

   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   // Find starting char
   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      	break;

      // search for first char in string
      if ( g_oStringBuffer[i] != 32 && 	// space
           g_oStringBuffer[i] != 9  &&		// tab
           g_oStringBuffer[i] != 10 &&    // newline
           g_oStringBuffer[i] != 13 &&    // car return
           g_oStringBuffer[i] != 11  )    // vert tab
      {
      	iStartLoc = i;
      	cpStartString = & g_oStringBuffer[i];
			break;
      }
   }

      // Find stopping char
   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      {
			i--;
      	break;
      }
   }
   for ( ; i > iStartLoc; i--)
   {
      // Ignore these chars
      if ( g_oStringBuffer[i] != 32 && 	// space
           g_oStringBuffer[i] != 9  &&		// tab
           g_oStringBuffer[i] != 10 &&    // newline
           g_oStringBuffer[i] != 13 &&    // car return
           g_oStringBuffer[i] != 11  )    // vert tab
      {
      	// Set new ending of string
      	g_oStringBuffer[i+1] = '\0';
			break;
      }
   }

	strncpy ( g_oStringBuffer2, cpStartString, PHP_MAX_STRINGLENGTH );

   SaveVariableString( pServer, pReturn, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );
   return 0;
}


/*** BeginHeader ltrim_function */
int ltrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
ltrim_function                   <PHP_LIB1.LIB>

SYNTAX: int ltrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:
ltrim --  Strip whitespace from the beginning of a string

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int ltrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int i;
	auto char * cpStartString;

   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   // Find starting char
   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      	break;

      // search for first char in string
      if ( g_oStringBuffer[i] != 32 && 	// space
           g_oStringBuffer[i] != 9  &&		// tab
           g_oStringBuffer[i] != 10 &&    // newline
           g_oStringBuffer[i] != 13 &&    // car return
           g_oStringBuffer[i] != 11  )    // vert tab
      {
      	cpStartString = & g_oStringBuffer[i];
			break;
      }
   }

	strncpy ( g_oStringBuffer2, cpStartString, PHP_MAX_STRINGLENGTH );

   SaveVariableString( pServer, pReturn, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );
   return 0;
}


/*** BeginHeader rtrim_function */
int rtrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
rtrim_function                   <PHP_LIB1.LIB>

SYNTAX: int rtrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:
rtrim --  Strip whitespace from the end of a string

PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int rtrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int i;

   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   // Find stopping char
   for ( i = 0; i < PHP_MAX_STRINGLENGTH; i++)
   {
    	if ( g_oStringBuffer[i] == '\0' )
      {
      	i--;
      	break;
      }
   }
   for ( ; i > 0; i--)
   {
      // Ignore these chars
      if ( g_oStringBuffer[i] != 32 && 	// space
           g_oStringBuffer[i] != 9  &&		// tab
           g_oStringBuffer[i] != 10 &&    // newline
           g_oStringBuffer[i] != 13 &&    // car return
           g_oStringBuffer[i] != 11  )    // vert tab
      {
      	// Set new ending of string
      	g_oStringBuffer[i+1] = '\0';
         break;
      }
   }

	strncpy ( g_oStringBuffer2, g_oStringBuffer, PHP_MAX_STRINGLENGTH );

   SaveVariableString( pServer, pReturn, g_oStringBuffer2, PHP_MAX_STRINGLENGTH );
   return 0;;
}

/*** BeginHeader strstr_function */
int strstr_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strstr_function                   <PHP_LIB1.LIB>

SYNTAX: int strstr_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:
Returns part of haystack string from the first occurrence of needle to the end of haystack.

If needle is not found, returns FALSE.

If needle is not a string, it is converted to an integer and applied as the ordinal value of a character.


PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strstr_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
   auto int iNum;
   auto int i;
   auto char * cpReturn;

   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );

   if ( pServer->php_interpreter.oVariableTable[iArgList[0]].cVarType == VARTYPE_STRING )
   {
   	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), g_oStringBuffer2 );
      cpReturn = strstr( g_oStringBuffer, g_oStringBuffer2 );
   }
   else
   {
      iNum = ConvertVariableToInt(&(pServer->php_interpreter.oVariableTable[iArgList[i]]));
      cpReturn = strchr( g_oStringBuffer, (char) iNum );
   }

   if ( cpReturn == NULL )
   {
 	   pReturn->cVarType = VARTYPE_INT;
	   pReturn->VarData.iIntValue = FALSE;
      return 0;
   }
   else
   {
	   strncpy(g_oStringBuffer, cpReturn, PHP_MAX_STRINGLENGTH);
	   SaveVariableString( pServer, pReturn, g_oStringBuffer, PHP_MAX_STRINGLENGTH );
	   return 0;
   }
}


/*** BeginHeader strcasecmp_function */
int strcasecmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
/*** EndHeader */

/* START FUNCTION DESCRIPTION ********************************************
strcasecmp_function                   <PHP_LIB1.LIB>

SYNTAX: int strcasecmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );

KEYWORDS:		tcpip, http, processor, front-end

DESCRIPTION:
<0: str1 is less than str2 because
character in str1 is less than corresponding character in str2, or
str1 is shorter than but otherwise identical to str2.


=0: str1 is identical to str2


>0: str1 is greater than str2 because
character in str1 is greater than corresponding character in str2, or
str2 is shorter than but otherwise identical to str1.


PARAMETER1:	HTTP Server Structure.

RETURN VALUE:  Test Function for the internal functions

SEE ALSO:

END DESCRIPTION **********************************************************/
_phplib1_nodebug int strcasecmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount )
{
	ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[0]]), g_oStringBuffer );
   ConvertVariableToString( &(pServer->php_interpreter.oVariableTable[iArgList[1]]), g_oStringBuffer2 );


   pReturn->cVarType = VARTYPE_INT;
   pReturn->VarData.iIntValue = strcmpi(g_oStringBuffer , g_oStringBuffer2);

   return 0;
}
