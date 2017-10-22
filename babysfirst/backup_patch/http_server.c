/*******************************************************************************
********************************************************************************
* Team Rabbit Core Senior Design Project 				Copyright (c) 2004
*
* Group Members:
* 		Jason Williams		(alias JW)
*     Duong Nguyven		(alias DN)
*		Phuoc Nguyven		(alias PN)
*
* File Name: http_server.c
*
* Description:
*		JW 5/20/04 - Just created this file working with the datastructures and
* setting up the server and getting it to work with just basic pages first.
*
* Revision:
*		JW 5/20/04 - 1.0
*
********************************************************************************
*******************************************************************************/

#include "my_http.h"
#include "tokenizer.h"
#include "web_files.h"

#define auto	

/*
 * JW - Set the network to use DHCP.
 * when it becomes a library.
 */
#define TCPCONFIG 3

// #define DEBUG_ALL_CODE

#ifdef DEBUG_ALL_CODE
/* Debug defines follow for each library */
#define RC_MPARSER_DEBUG      //  main_parser.lib
#define RC_MPARSER_VERBOSE

#define RC_HTTP_DEBUG 			// my_http.lib
#define HTTP_VERBOSE

#define RC_PHPVAR_DEBUG			// php_vars.lib
#define RC_PHPVAR_VERBOSE

#define RC_FUNCTION_DEBUG		// php_functions.lib
#define RC_FUNCTION_VERBOSE

#define RC_TOKEN_DEBUG			// tokenizer.lib

#define RC_PHPLIB1_DEBUG		// php_lib1.lib
#define RC_PHPLIB1_VERBOSE

#define RC_PHPLIB2_DEBUG		// php_lib2.lib
#define RC_PHPLIB2_VERBOSE

#define RC_PHPLIBSPC_DEBUG 	// php_libpsc.lib
#define RC_PHPLIBSPC_VERBOSE

#define RC_DATALOG_DEBUG		// data_logger.lib

#endif

// #define RC_DATALOG_DEBUG


char *g_pUploadPHPFile;




/*
 * Note: When you ximport a file - the first 4 bytes contain the length of the
 * file then the following bytes after that are the contents of the file.
 *
 */
/*
#ximport "page.txt"    	test_page
#ximport "page.txt"		test_html
#ximport "test.gif"		test_gif
#ximport "form.txt"		test_form
*/

/*
#ximport "index.php" 		index_page
#ximport "XMLData.php"     xmldata_page
#ximport "dog.jpg"			dogjpg_page
#ximport "dog1.jpg"			dogjpg1_page
#ximport "mainPHP1.jpg"		mainphp1_page
#ximport "mainPHP2.jpg"		mainphp2_page
#ximport "maintitle.jpg"	maintitle_page
#ximport "milestone.gif"	milestone_page
#ximport "RCM_pic.jpg"		rcmpic_page
#ximport "rooster.jpg"		rooster_page


#memmap xmem
#use "dcrtcp.lib"
#use "my_http.lib"
#use "php_vars.lib"
#use "tokenizer.lib"
#use "php_lib1.lib"
#use "php_lib2.lib"
#use "data_logger.lib"
#use "php_libspc.lib"
#use "php_functions.lib"
#use "main_parser.lib"
*/

/******************************************************************************
Testing
******************************************************************************/

/*******************************************************************************
***** File System Data Structures ****
*******************************************************************************/
#define HTTP_FILE_MAXNAME	40

typedef struct
{
   char szTypeExtension[10];
   char szTypeName[40];
   int (*mime_handler)( HttpServerStruct *, struct HttpFile *, struct HttpMimeType * );
} HttpMimeType;

typedef struct
{
   char szFileName[HTTP_FILE_MAXNAME];
   char *file_physical_addr;
   int file_size;
   int mime_index;
} HttpFile;

/*******************************************************************************
***** Function Declarations
*******************************************************************************/
int php_file_handler( HttpServerStruct *pServer, HttpFile *pFileStruct, HttpMimeType *pMimeStruct )
{
   char more_hdrs;
   int iFileLength;
   //char input_buffer[512];

   //char *ptr;
   TokenStruct T1;
   int i;
   int len;

   more_hdrs = 0;

   // Generate Header
   len = 0;

   len += sprintf( pServer->inout_buffer,
      	"HTTP/1.%c %d %s\r\nDate: %s\r\nServer: defcon1\r\nConnection: close\r\nContent-Type: %s\r\n%s", pServer->http_version == HTTP_VER_11 ? '1' : '0'
        , 200
        , "OK"
        , "Sun, 10 Jul 2017 11:12:30 GMT"
        , pMimeStruct->szTypeName
        , more_hdrs ? "" : "\r\n"
        );

	http_flush_header( pServer, len );

   //ptr = input_buffer;

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: send_phpfile_handler: Setting up sliding window!\n" );
#endif

   // Setup the servers file buffers sliding window for the php interpreter.
   pServer->php_interpreter.iFileLength = pFileStruct->file_size;
   // xmem2root( pServer->php_interpreter.szFileBuffer, (pFileStruct->file_physical_addr + 4),
   //            pServer->php_interpreter.iFileLength );

   // pServer->php_interpreter.pFileLocation = pServer->php_interpreter.szFileBuffer;

   // Setup the sliding window buffer for reading the file.
   pServer->php_interpreter.ppFileLocation = pFileStruct->file_physical_addr;
   pServer->php_interpreter.ppFileStartLocation = pFileStruct->file_physical_addr;
   pServer->php_interpreter.pFileData = pFileStruct->file_physical_addr;
   //pServer->php_interpreter.pFileLeftBoundary = ((pServer->php_interpreter.szFileBuffer + (PHP_MAX_FILEBUFFER / 2)) - (PHP_MAX_FILEBUFFER / 4));
   //pServer->php_interpreter.pFileRightBoundary = ((pServer->php_interpreter.szFileBuffer + (PHP_MAX_FILEBUFFER / 2)) + (PHP_MAX_FILEBUFFER / 4));

   // Setup the sliding window
   MoveSlidingWindow( pServer, TRUE );

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: send_phpfile_handler: Calling front end processor!\n" );
#endif

   // Perform the PHP Interpretation.
   FrontEndProcessor( pServer );

   // Data may still be in the inout buffer, finish writing it.
   http_flush_inout_buffer( pServer );


   // Inform the server we are done and want to send the page
   pServer->state = HTTP_SENDPAGE;
   pServer->output_offset = 0;
   pServer->header_offset = 0;

   // Set the output buffer to the reserved location of the buffer - this
   // may change if the output buffer points to an xmem file.
   pServer->output_buffer_loc = pServer->output_buffer_reserved_loc;

   return 0;
}

int send_xmemfile_handler( HttpServerStruct *pServer, HttpFile *pFileStruct, HttpMimeType *pMimeStruct )
{
   auto char more_hdrs;
   auto int len;

   more_hdrs = 0;

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: send_xmemfile_handler!\n" );
#endif

   // Generate Header
   len = 0;

   len += sprintf( pServer->inout_buffer,
      	"HTTP/1.%c %d %s\r\nDate: %s\r\nServer: defcon1\r\nConnection: close\r\nContent-Type: %s\r\n%s"
        , pServer->http_version == HTTP_VER_11 ? '1' : '0'
        , 200
        , "OK"
        , "Sun, 10 Jul 2017 11:12:30 GMT"
        , pMimeStruct->szTypeName
        , more_hdrs ? "" : "\r\n"
        );

	http_flush_header( pServer, len );



	// Get the length of the file in xmem
   len = pFileStruct->file_size;

   // Point the output buffer to the files physical location in memory
   pServer->output_buffer_loc = pFileStruct->file_physical_addr;
   pServer->output_length = len;

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: send_xmemfile_handler: output_length = %d, output_buffer_loc = %x\n", pServer->output_length, pServer->output_buffer_loc );
#endif



   // Inform the server we are done and want to send the page
   pServer->state = HTTP_SENDPAGE;
   pServer->output_offset = 0;
   pServer->header_offset = 0;

   return 0;
}


/*******************************************
** The MIME Type Handler table - evaluates
** file extensions into function handlers
** that process the page and send the output
** into the content buffer
********************************************/
#define HTTP_MIME_TABLE_SIZE	5
const HttpMimeType http_mime_table[HTTP_MIME_TABLE_SIZE] =
{
	{ ".php", "text/html", &php_file_handler },
   { ".htm", "text/html", &send_xmemfile_handler },
   { ".gif", "image/gif", &send_xmemfile_handler },
	{ ".jpg", "image/jpeg", &send_xmemfile_handler },
   { ".php", "text/xml", &php_file_handler }
};


/*******************************************
** The HTTP File Table - Contains the list
** of files that are in the root directory
** of the web server and the physical
** location in flash memory.
********************************************/

/*
#define HTTP_FILE_TABLE_SIZE	8
const HttpFile http_file_table[] =
{
	{ "/index.php", index_page, &http_mime_table[0] },
   { "/XMLData.php", xmldata_page, &http_mime_table[4] },
   { "/dog.jpg", dogjpg_page,	&http_mime_table[3] },
   { "/dog1.jpg", dogjpg1_page, &http_mime_table[3] },
   { "/mainPHP1.jpg", mainphp1_page, &http_mime_table[3] },
   { "/mainPHP2.jpg", mainphp2_page, &http_mime_table[3] },
   { "/maintitle.jpg", maintitle_page, &http_mime_table[3] },
   { "/milestone.gif", milestone_page, &http_mime_table[2] },
   { "/RCM_Pic.jpg", rcmpic_page, &http_mime_table[3] },
   { "/rooster.jpg", rooster_page, &http_mime_table[3] }
};
*/
/*
#define HTTP_FILE_TABLE_SIZE	4
const HttpFile http_file_table[] =
{
	{ "/index.php", test_page, &http_mime_table[0] },
   { "/test.htm", test_html, &http_mime_table[1] },
   { "/test.gif", test_gif, &http_mime_table[2] },
   { "/form.php", test_form, &http_mime_table[0] }
};
*/

#define HTTP_UPLOAD_NAMED_PHP_FILE_LOC	(2)
#define HTTP_UPLOAD_PHP_FILE_LOC			(3)

#define HTTP_FILE_TABLE_SIZE	7
HttpFile http_file_table[HTTP_FILE_TABLE_SIZE] =
{
   { "/test.htm", g_TestHTM, sizeof(g_TestHTM), 1 },
   { "/test.php", g_TestPHP, sizeof(g_TestPHP), 0 },
	{ "/upload/empty.htm", NULL, 0, 1 },
	{ "/upload/user.php", NULL, 0, 0 }, 	// HTTP_UPLOAD_PHP_FILE_LOC must match this index
	{ "/upload.php", g_UploadPHP, sizeof(g_UploadPHP), 0 },
	{ "/index.htm", g_IndexHTM, sizeof(g_IndexHTM), 1 },
	{ "/test/get.php", g_TestGETPHP, sizeof(g_TestGETPHP), 0 }
	// { "/test/post.php", g_TestPOSTPHP, sizeof(g_TestPOSTPHP), 0 }
};


/*******************************************************************************
********************************************************************************
*******************************************************************************/


/*******************************************************************************
********************************************************************************
*******************************************************************************/


int http_process( HttpServerStruct* pServer )
{
	auto int iIndex;
   int fileIndex;
   int mimeIndex;
   auto int len;
   auto char bFound;

#if HTTP_VERBOSE
   printf( "Calling HTTP Process Function. (%x)\n", pServer->header_buffer_pp );
#endif

   bFound = 0;
   for ( iIndex = 0; iIndex < HTTP_FILE_TABLE_SIZE; iIndex++ )
   {
      if ( strcmp( pServer->request_url, http_file_table[iIndex].szFileName ) == 0 )
      {
	 fileIndex = iIndex;
         bFound = 1;
         break;
      }
   }

   if ( !bFound )
   {
#if HTTP_VERBOSE
   	printf( "HTTP Process - File Not Found: %s (%x)\n", pServer->request_url, pServer->header_buffer_pp );
#endif

   	// Send HTTP 404 error.
      // Generate Header
   	len = 0;

   	len += sprintf( pServer->inout_buffer,
      	"HTTP/1.%d %d %s\r\nDate: %s\r\nServer: defcon1\r\nConnection: close\r\nContent-Type: text/html\r\n%s"
        , pServer->http_version == HTTP_VER_11 ? 1 : 0
        , 404
        , "Not Found"
        , "Sun, 10 Jul 2017 11:12:30 GMT"
        , "\r\n"
        );

#if HTTP_VERBOSE
	printf( "GOT HERE\n" );
  	printf( "HTTP Process - Flush Header (%s) (%x)\n", pServer->inout_buffer, pServer->header_buffer_pp );
#endif
	http_flush_header( pServer, len );

      pServer->inout_buffer_length = 0;
      http_write_output( pServer, "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD><BODY>404 Not Found</BODY></HTML>", strlen( "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD><BODY>404 Not Found</BODY></HTML>" ) );
      http_flush_inout_buffer( pServer );

#if HTTP_VERBOSE
   	printf( "HTTP Process - Flush inout\n" );
#endif
      // Inform the server we are done and want to send the page
   	pServer->state = HTTP_SENDPAGE;
   	pServer->output_offset = 0;
   	pServer->header_offset = 0;

      // Set the output buffer to the reserved location of the buffer - this
      // may change if the output buffer points to an xmem file.
      pServer->output_buffer_loc = pServer->output_buffer_reserved_loc;

      return 0;
   }

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: Webpage found!\n" );
#endif

   mimeIndex = http_file_table[fileIndex].mime_index;

#if HTTP_VERBOSE
   printf( "HTTP_PROCESS: File Struct: file_size = %d, %d, %x, %s\n", http_file_table[fileIndex].file_size, mimeIndex, http_mime_table[mimeIndex].mime_handler, http_mime_table[mimeIndex].szTypeExtension );
#endif

   return (*http_mime_table[mimeIndex].mime_handler)( pServer, &(http_file_table[fileIndex]), &(http_mime_table[mimeIndex]) );
}


/*
void separator();

void separator()
{
	printf(
	 " \xC3-----------------------------------------------------------------------------\xB4"
	 );
}
*/

void SetUploadPHPFile( char *pData, int length )
{
	if ( http_file_table[HTTP_UPLOAD_PHP_FILE_LOC].file_physical_addr )
		free( http_file_table[HTTP_UPLOAD_PHP_FILE_LOC].file_physical_addr );

	http_file_table[HTTP_UPLOAD_PHP_FILE_LOC].file_physical_addr = pData;
	http_file_table[HTTP_UPLOAD_PHP_FILE_LOC].file_size = length;
}

void SetNamedUploadPHPFile( char *pData, int length, char *pFilename )
{
	if ( http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].file_physical_addr )
		free( http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].file_physical_addr );

	sprintf( http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].szFileName, "/upload/%s.htm", pFilename );

	//printf( "FILE: %s\n", http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].szFileName );

	http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].file_physical_addr = pData;
	http_file_table[HTTP_UPLOAD_NAMED_PHP_FILE_LOC].file_size = length;
}

void main()
{
	int a;
	int sz;
	char *ppHttpServerAddress;
	unsigned int t0;

   a = 0;

	char *pTempData = malloc( sizeof(g_uploadPHPTempFile) );
	
	memcpy( pTempData, g_uploadPHPTempFile, sizeof(g_uploadPHPTempFile) );
	
	SetUploadPHPFile( pTempData, sizeof(g_uploadPHPTempFile) );

	/*
	pTempData = malloc( sizeof(g_PostTestPHPFile) );
	
	memcpy( pTempData, g_PostTestPHPFile, sizeof(g_PostTestPHPFile) );

	SetNamedUploadPHPFile( pTempData, sizeof(g_PostTestPHPFile), "empty" );
	*/

   // Initialize
   http_init( http_process );
   Tokenizer_Init();



/*
 *  http_handler needs to be called to handle the active http servers.
 */

   while (1)
   {
      	http_server( );
        
	//waitfor(DelayMs( 1000 )); // Attempt to delay exactly 1second
   }
}
