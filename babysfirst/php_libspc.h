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

#include "my_http.h"

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




int post_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
int get_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
int cookie_data_spcfunc( HttpServerStruct *pServer, PHPVariable *pReturn, int iArg );
int setcookie_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int uploadfile_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
