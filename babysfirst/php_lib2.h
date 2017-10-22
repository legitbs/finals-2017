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
#include "my_http.h"


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
