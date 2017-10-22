/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: http_server.lib
*
* Description:
*		JW 5/20/04 - Just created this file working with the datastructures and
* setting up the server and getting it to work with just basic pages first.
*
* Revision:
*		JW 5/20/04 - 1.0
*
* 		PN 7-12		Added char cForLoopCounter in HttpServerStruct
********************************************************************************
*******************************************************************************/

#include "out_helper.h"

#ifndef auto
#define auto
#endif

#ifndef NULL
#define NULL (0)
#endif

// Remove HTTP_VERBOSE after testing
//#define HTTP_VERBOSE	(1)

/*** BeginHeader */

/*
 * JW - Allow debugging options if specified.  This will help reduce code space.
 */
#ifdef RC_HTTP_DEBUG
	#define _http_nodebug debug
	#define _http_assert(exp) assert(exp)
#else
	#define _http_nodebug nodebug
	#define _http_assert(exp)			// Define to nothing
#endif


/*
 * JW - Web server configuration
 */
#ifndef HTTP_MAXSERVERS
	#define HTTP_MAXSERVERS 1
#endif

#ifndef MAX_TCP_SOCKET_BUFFERS
	#define MAX_TCP_SOCKET_BUFFERS 1
#endif

/*
 * Bitfield macros
 */
#define SET_BIT(bitfield,bits) ((bitfield) = (bitfield) | (bits))
#define REMOVE_BIT(bitfield,bits) ((bitfield) = (bitfield) & ~(bits))
#define IS_SET(bitfield,bits) ((bitfield) & (bits))

/*
 * Notes on Buffers:
 *		The inout_buffer (HTTP_MAX_INOUTBUFFER defines its maximum size) stores in
 * the input line from the HTTP Server and also stores a block of output that
 * is to be written out by the HTTP Server.  The inout_buffer holds temporarily
 * the data to be written and is flushed to XMEM where the entire output is
 * located.  Then the HTTP Server writes back out in chunks the XMEM using
 * the inout_buffer.
 */
#define HTTP_MAX_INOUTBUFFER		8192
#define HTTP_MAX_OUTPUTBUFFER		8192
#define HTTP_MAX_HEADERBUFFER		8192
#define HTTP_MAXURL					512
#define HTTP_MAXNAME					256
#define HTTP_COOKIE_BUFFER			2048
#define HTTP_GETPOST_BUFFER		4096

#define PHP_MAX_FILEBUFFER			4032 		// Needs to be evenly divisible by 2 and 3!
#define PHP_MAX_FUNCTIONTABLE		50
#define PHP_MAX_JUMPBACKS			255
#define PHP_MAX_VARIABLETABLE		200
#define PHP_MAX_STRINGLENGTH		128
#define PHP_MAX_VARNAME_SIZE		19

// Global String Buffer
extern char g_oStringBuffer[PHP_MAX_STRINGLENGTH+1];
extern char g_oStringBuffer2[PHP_MAX_STRINGLENGTH+1];

/*
 * 	Max time (in seconds) that HTTP will remaining in any connected state.
 *    After timeout, the Rabbit will (force) close the connection.
 */
#ifndef HTTP_TIMEOUT
   #define HTTP_TIMEOUT		200// 16
#endif

#ifndef HTTP_PORT
	#define HTTP_PORT 80
#endif

#ifndef HTTP_IFACE
	#define HTTP_IFACE	IF_ANY
#endif


/***********************
 * HTTP SERVER STATES
 *
 ***********************/
#define HTTP_INIT         		0		// Server was just initialized
#define HTTP_LISTEN       		1
#define HTTP_GETREQ       		2
#define HTTP_GETHEAD      		3
#define HTTP_HANDLEPOST			4
#define HTTP_PROCESS      		5
#define HTTP_SENDPAGE     		6
#define HTTP_HANDLER      		7
#define HTTP_WAITCLOSE    		8
#define HTTP_FINISHWRITE  		9
#define HTTP_DIE          		10

#define HTTP_METHOD_GET   		1
#define HTTP_METHOD_HEAD  		2
#define HTTP_METHOD_POST  		3

#define HTTP_VER_09       		1
#define HTTP_VER_10       		2
#define HTTP_VER_11       		3

#define FORM_STATE_NONE			0
#define FORM_STATE_HASFORM		1
#define FORM_STATE_COPYNAME	2
#define FORM_STATE_COPYVAL		3
#define FORM_STATE_DONE			4
#define FORM_STATE_ERROR		5
// 0 = no form, 1 = has a form, 2 = copy name, 3 = copy value, 4 = done, 5 = error

#define COOKIE_STATE_COPYNAME	0
#define COOKIE_STATE_COPYVAL	1
#define COOKIE_STATE_DONE		2
#define COOKIE_STATE_ERROR		3

// Content-Disposition types
#define MIME_DISP_NONE			0
#define MIME_DISP_INLINE		1
#define MIME_DISP_ATTACHMENT	2
#define MIME_DISP_FORMDATA		3	// only this one is really relevant to us.

/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */
//#define REDIRECTHOST		_PRIMARY_STATIC_IP
//#define REDIRECTHOST	"proxy.domain.com:1212"


/********************************
 * End of configuration section *
 ********************************/

/* where to send the user after the CGI finishes */
//#define REDIRECTTO 		"http://" REDIRECTHOST "/index.html"

//#define TIMEZONE        -8
typedef struct HttpServerStruct;

typedef struct {
   char *pFuncName;
   int iFuncNameLen;

   char *ppFunctionLocation;

   unsigned int iArgLen;
} PHPFunction;

/*
 * PHP Variable Data Structure
 *
 */
typedef struct {
   int cVarType;

   char szVarName[PHP_MAX_VARNAME_SIZE+1];
   int iVarNameLen;

   int iScope;

   int iVNum;				// The VNum or offset in the area for this variable.
   int iNext;				// Next in either the free list or the
   							// variable list

   union {
      int iIntValue;
      //float fFloatValue;
      char *ppStringValue;
   } VarData;
} PHPVariable;

typedef struct {
   unsigned int ucOperatorSymbol;
   int cOperatorType;
} PHPOperator;

typedef struct
{
   char *ppJumpBackLocation;
   unsigned int cJumpBackType;
   unsigned int cScope;

} PHPJumpBackEntry;

/*******************************************************************************
***** PHP Interpreter Data Structure
*******************************************************************************/

typedef struct {
/* Sliding window for the file buffer - to conserve root data */
   //char szFileBuffer[PHP_MAX_FILEBUFFER];		// The file buffer for the PHP File
   char *ppFileLocation;								// The location in the file
   //char *ppWindowLocation;							// The location of the sliding window.
   char *ppFileStartLocation;						// The starting location in the file (not changed).
   int iFileLength;									// The length of the file
   char *pFileData;									// The current location of the file data (in the window).
   //char *pFileLeftBoundary;						// The left boundary in the file buffer (never changes).
   //char *pFileRightBoundary;						// The right boundary in the file buffer (never changes).

/* Variable Table */
   PHPVariable oVariableTable[PHP_MAX_VARIABLETABLE];
   int iFreeList;										// The first element in the free variable list
   int iVarList;										// The first element in the allocated variable list
   int iVarTableSize;								// The size of the variable table
   char *ppStringVarBufLoc;							// The physical location of the string buffers for variables

/* Function Table */
   PHPFunction oFunctionTable[PHP_MAX_FUNCTIONTABLE];
   int iTopFunction;

/* Jump back addresses for loops and function calls */
   PHPJumpBackEntry oJumpBackTable[PHP_MAX_JUMPBACKS];
   int iTopJumpEntry;


   int iCurrentScope;		// The current scope of this code block
   int iFunctionScope;		// The current scope (if in a function call) a function block is defined to be in.

   /* Allow for loop to reconize first loop */
   int cForLoopCounter;

   /* Ignore embedded html */
   int bIgnoreHTMLCode;

} PHPInterpreterStruct;


typedef struct {
   /* Server state variable */
   int state;
   int laststate;
   int nextstate;

   /* PHP Interpreter variable */
	/*
    * JW - Whenever the PHP Interpreter is written we'll want to set add this back into the data structure.
    */
   PHPInterpreterStruct php_interpreter;


   char inout_buffer[HTTP_MAX_INOUTBUFFER];	// The input/output buffer

   /* Input variables */
   int input_length;
   char *input_pos;

   /* Buffered input/output writing during cgi processing variables */
   int inout_buffer_length;	// The current length in the inout_buffer

   /* Output variables */
   char *output_buffer_reserved_loc;	// The reserved location for the output buffer.
   char *output_buffer_loc;			// The physical address of the output buffer
   int output_length;				// The length of the output in physical memory
   int output_offset;				// The current location in physical memory
   										// where the output has left off.

   char *header_buffer_pp;			// The physical address of the header buffer
   int header_length;				// The length of the header in physical memory
   int header_offset;				// The current location in physical memory
   										// where the header has left off.

   /* Timeout variables */
	int main_timeout;         // Timeout value for the state machine

   /* Errors */
	int error_state;      		// Error state variable for the server

   /* Http request and header info */
	int request_method;             // The method of the request (GET/POST/etc)
	char request_url[HTTP_MAXURL];  	// The requested URI
	int http_version;         // The version of the request
	char content_type[40];     // The type of the content
	int content_length;    	// The length of the content

   /* Basic authentication */
	char username[HTTP_MAXNAME];
	char password[HTTP_MAXNAME];

   /* Cookie buffer for holding Netscape Cookies */
	char cookie_buffer[HTTP_COOKIE_BUFFER];
   char *cookie_stack_end;    // End of the cookie data stack

   /* GET/POST buffer for holding get/post data */
	char get_post_buffer[HTTP_GETPOST_BUFFER];
	char *get_stack_top;       // Top of the get data stack
   char *get_stack_end;       // End of the get data stack
	char *post_stack_top;      // Top of the post data stack
   char *post_stack_end;      // End of the post data stack

   /* HTTP Upload */
   int part_content_length;	// Content length of this part
	int action;					// Current action code for CGI (CGI_START etc.)
	int transfer_encoding;		// Content-Transfer-Encoding (CTE_* definitions)
   int disposition;				// Content-Disposition type (MIME_DISP_* definitions)
   int got_boundary;			// If true, just read the end of a boundary string
   int boundary_len;			// Length of boundary string (including initial \r\n--)
	char boundary[74];			// Boundary separator for multipart/form-data.  Includes
   									// initial "\r\n--".  Not null term.

   char tag_name[HTTP_MAXNAME];	// The name of the POST request block (we ignore file names)
   char tag_filename[HTTP_MAXNAME];	// The filename for a post request

   /* Http Process Function for this server */
   int (*http_process_handler)( );

} HttpServerStruct;

extern HttpServerStruct *http_servers;

/*** EndHeader */

/*******************************************************************************
 *******************************************************************************
 *
 *    INTERNAL UTILITY FUNCTIONS
 *
 *******************************************************************************
 ******************************************************************************/


/*** BeginHeader http_urldecode */
char *http_urldecode(char *dest, /*const*/char *src, int len);
int http_scanpost(char* tag, char* buffer, char* dest, int maxlen);
void http_date_str(char *buf);
unsigned char base64char(char c);
void unbase64(char *dest, char *src);
int http_getline(HttpServerStruct *pServer);
int http_parseget( HttpServerStruct *pServer );
int http_parsepost( HttpServerStruct *pServer );
int http_parsehead(HttpServerStruct* pServer, int part);
void _http_abort(int x);
int http_init( int (*pHandlerFunction)( ) );
int http_server( void );
int http_flush_header( HttpServerStruct *pServer, int iLength );
int http_write_output( HttpServerStruct *pServer, char *pString, int iLength );
int http_flush_inout_buffer( HttpServerStruct *pServer );
int MoveSlidingWindow( HttpServerStruct *pServer, char bForceMove );

void SetUploadPHPFile( char *pData, int length );
