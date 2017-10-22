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

#include "my_http.h"

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




int test_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int pow_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int srand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int rand_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strlen_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strcmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strncmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strtolower_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strtoupper_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int setdatetime_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int getdate_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int is_int_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int is_float_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int is_string_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int trim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int ltrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int rtrim_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strstr_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
int strcasecmp_function( HttpServerStruct *pServer, PHPVariable *pReturn, int iArgList[], int iArgCount );
