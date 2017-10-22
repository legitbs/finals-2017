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
#include "my_http.h"
#include "tokenizer.h"
#include "out_helper.h"

#include <string.h>
#include <time.h>

HttpServerStruct *http_servers;

char g_oStringBuffer[PHP_MAX_STRINGLENGTH+1];
char g_oStringBuffer2[PHP_MAX_STRINGLENGTH+1];

/* START FUNCTION DESCRIPTION ********************************************
http_urldecode                   <HTTP.LIB>

SYNTAX: char *http_urldecode(char *dest, const char *src, int len);

KEYWORDS:		tcpip, http, transfer-coding

DESCRIPTION:	Converts a string with URL escaped ``tokens''
		(such as %20 (hex) for space) into actual values.  Changes "+"
		into a space.  String can be NUL byte terminated; it is also
		bounded by a specified string length.  This function is reentrant.

PARAMETER1:	Buffer where decoded string is stored.
PARAMETER2:	Buffer holding original string (not changed).
PARAMETER3: maximum size of string (NUL terminated strings can be shorter).

RETURN VALUE:	If all conversion are good, returns 'dest', or NULL if some
		some conversion had troubles.

SEE ALSO:	http_contentencode()

END DESCRIPTION **********************************************************/
_http_nodebug char *http_urldecode(char *dest, /*const*/char *src, int len)
{
	auto int i;
	auto int j;
	auto char upper;
	auto char lower;
	auto int error;
	auto char lc;
	auto int lenm1;

	i = 0;
	j = 0;
	error = 0;

	if (len < 0)
   {
		len = -len;
		lc = 1;
	}
	else
		lc = 0;

	lenm1 = len-1;

	while( (i < lenm1) && (src[i] != '\0') )
   {
		if (src[i] == '+')
      {
			dest[j] = ' ';
		}
      else if (src[i] == '%')
      {
			/* "%" introduces a 2 digit hex number representing a char.
			 *  If not two more chars, flag error. If two digits are NOT
			 *  valid hex digits, also flag an error.
			 */
			if ((i + 2) >= len)
         {
				error = 1;
				break;
			}
			upper = toupper(src[++i]);
			lower = toupper(src[++i]);

			if ((upper >= '0') && (upper <= '9'))
         {
				upper = upper - '0';
			}
         else if ((upper >= 'A') && (upper <= 'F'))
         {
				upper = upper - 'A' + 10;
			}
         else
         {
				error = 1;
				break;
			}

         if ((lower >= '0') && (lower <= '9'))
         {
				lower = lower - '0';
			}
         else if ((lower >= 'A') && (lower <= 'F'))
         {
				lower = lower - 'A' + 10;
			}
         else
         {
				error = 1;
				break;
			}

			dest[j] = (upper << 4) + lower;
		}
      else
      {
			/* No conversion, so copy char verbatim. */
			dest[j] = src[i];
		}

      if (lc)
			dest[j] = tolower(dest[j]);

      i++;
		j++;
	}
	dest[j] = '\0';
	if ((i == lenm1) && (src[i] != '\0'))
   {
		error = 1;
	}
	if (error == 0)
   {
		return dest;
	}
   else
   {
		return NULL;
	}
}


/* START FUNCTION DESCRIPTION ********************************************
http_scanpost                        <HTTP.LIB>

SYNTAX: int http_scanpost(char* tag, char* buffer, char* dest, int maxlen);

KEYWORDS:		tcpip, http, post

DESCRIPTION:	The http_scanpost function allows you to scan a buffer with
					a POST response for the key/value pairs.  Note that this
					function also does URL-decoding of the values.

					This function is reentrant.

PARAMETER1:	Buffer holding the tag name.
PARAMETER2:	Buffer to read data from.
PARAMETER3: Buffer to store value to.
PARAMETER4: size of destination buffer.

RETURN VALUE:	zero if successful, non-zero otherwise.

END DESCRIPTION **********************************************************/

_http_nodebug
int http_scanpost(char* tag, char* buffer, char* dest, int maxlen)
{
	auto int x, state, offset, length;

	state=offset=x=0;
	length=strlen(buffer);

	while(offset<length)
   {
		switch(state)
      {
			case 0: /* match tag */
				if( buffer[offset]=='=' && tag[x]==0 )
            { /* do we have a match? */
					x=0;
					state=1;
					offset++;
				}
            else if( buffer[offset++] != tag[x++] )
            { /* skip to next tag? */
					state=2;
				}
				break;

			case 1: /* copy data */
				if( x<(maxlen-1) && buffer[offset]!='&' )
					dest[x++]=buffer[offset++];

				if( x==(maxlen-1) || offset==length || buffer[offset]=='&' )
            {
					dest[x]=0;
					http_urldecode(dest,dest,maxlen);
					return 0;
				}
				break;

			case 2: /* scan until next & */
				if( buffer[offset]=='&' ) /* reached next tag? */
					state=x=0;
				offset++;
				break;
		}
	}
	return 1;
}

/* START FUNCTION DESCRIPTION ********************************************
http_date_str                   		<HTTP.LIB>

SYNTAX: char *http_date_str(char *buf);

KEYWORDS:		tcpip, http

DESCRIPTION: 	Print the date (timezone adjusted) into the given buffer.
		This assumes there is room!

PARAMETER1:	The buffer to write the date into. This requires at
		lest 30 bytes in the destination buffer.

RETURN VALUE: 	A pointer to the string.

SEE ALSO: 	http_handler

END DESCRIPTION **********************************************************/
_http_nodebug void http_date_str(char *buf)
{
   //auto char wk[64], mth[64];

   // time( &curtime );
   //curtime = 1500420244L;

   //localtime( &t );
/*
   t.tm_wday = 0;
   t.tm_mon = 7;

   switch (t.tm_wday)
   {
   case 0: strcpy( wk, "Sun" ); break;
   case 1: strcpy( wk, "Mon" ); break;
   case 2: strcpy( wk, "Tue" ); break;
   case 3: strcpy( wk, "Wed" ); break;
   case 4: strcpy( wk, "Thu" ); break;
   case 5: strcpy( wk, "Fri" ); break;
   case 6: strcpy( wk, "Sat" ); break;
   }

   switch (t.tm_mon)
   {
   case 1: strcpy( mth, "Jan" ); break;
   case 2: strcpy( mth, "Feb" ); break;
   case 3: strcpy( mth, "Mar" ); break;
   case 4: strcpy( mth, "Apr" ); break;
   case 5: strcpy( mth, "May" ); break;
   case 6: strcpy( mth, "Jun" ); break;
   case 7: strcpy( mth, "Jul" ); break;
   case 8: strcpy( mth, "Aug" ); break;
   case 9: strcpy( mth, "Sep" ); break;
   case 10: strcpy( mth, "Oct" ); break;
   case 11: strcpy( mth, "Nov" ); break;
   case 12: strcpy( mth, "Dec" ); break;
   }
*/
   sprintf(buf, "%s, %d %s %d %d:%d:%d GMT",
           // wk, t.tm_mday, mth, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
           "Sun", 10, "Jul", 117 + 1900, 11, 12, 30);

#if HTTP_VERBOSE
	printf( "http_date_str: GOT DATE: %s\n", buf );
#endif
}

/*** BeginHeader base64char */
unsigned char base64char(char c);
/*** EndHeader */

/* decodes one character of the base64 alphabet */
_http_nodebug unsigned char base64char(char c)
{
	if(c>='A'&&c<='Z')
		return c-'A';
	else if(c>='a' && c<='z')
		return c-'a'+26;
	else if(c>='0' && c<='9')
		return c-'0'+52;
	else if(c=='+')
		return 62;
	else if(c=='/')
		return 63;

	return 0;
}

/* takes a four-byte string in 'src', decodes it, and stores the ascii in 'dest' */
_http_nodebug void unbase64(char *dest, char *src)
{
   dest[0] = (base64char(src[0]) << 2) + (base64char(src[1]) >> 4);
   dest[1] = ((base64char(src[1]) & 0xf) << 4) + (base64char(src[2]) >> 2);
   dest[2] = ((base64char(src[2]) & 0x3) << 6) + (base64char(src[3]));
}

static int get_my_line (char *buffer, int sz)
{
	char ch[10];
	int counter = 0;
	long retval;

	while ( counter < sz )
	{
		if ( (retval = read( ch, 1 )) < 0 )
			exit( -1 );

		if ( retval == 0 )
		{
			struct timespec tm1;
			struct timespec tm2;

			tm1.tv_sec = 0;
			tm1.tv_msec = 50;

			millisleep( &tm1, &tm2 );

			continue;
		}

		if ( ch[0] == '\n' )
			break;	
		
		buffer[counter] = ch[0];

		counter++;
	}

	buffer[counter] = '\0';

	return counter;
}

/* START FUNCTION DESCRIPTION ********************************************
http_getline                   <HTTP.LIB>

SYNTAX: int http_getline(HttpServerStruct *pServer);

KEYWORDS:		tcpip, http, transfer-coding

DESCRIPTION:	Reads a single line from the input stream and stores it in the
             HttpServerStruct.

PARAMETER1:	A pointer to the HTTP Server structure that is storing the data

RETURN VALUE:	The number of characters received.

SEE ALSO:	http_server()

END DESCRIPTION **********************************************************/
_http_nodebug int http_getline(HttpServerStruct *pServer)
{
	auto int len;

   /* is there data waiting yet? */
	// if ((len = http_sock_bytesready(state)) == -1) {
#if 0
   if ( (len = sock_bytesready( &(pServer->s) ) == -1) )
   {
		return 0;
   }
#endif
   
#if 0 

	if (len > HTTP_MAX_INOUTBUFFER)
   {
#ifdef HTTP_VERBOSE
		printf("HTTP: line too long (%d/%d)\n", len, HTTP_MAX_INOUTBUFFER);
#endif
		/* error - line too long */
		pServer->state = HTTP_DIE;
		return 0;
	}

	// http_sock_gets(state, state->p, HTTP_MAXBUFFER);
   sock_gets( &(pServer->s), pServer->input_pos, HTTP_MAX_INOUTBUFFER );
#ifdef HTTP_VERBOSE
	if (debug_on > 2)
		printf("----HTTP: getline: '%s'\n", pServer->input_pos);
#endif
#endif

	get_my_line( pServer->inout_buffer, HTTP_MAX_INOUTBUFFER-1 );
	pServer->input_pos = pServer->inout_buffer;

#ifdef HTTP_VERBOSE:
	printf( "HTTP: http_getline: %s\n", pServer->inout_buffer );
#endif

	return 1;
}

/* START FUNCTION DESCRIPTION ********************************************
http_parseget                   <HTTP.LIB>

SYNTAX: int http_parseget(HttpServerStruct *pServer);

KEYWORDS:		tcpip, http, transfer-coding

DESCRIPTION:	Parses the GET request string.

PARAMETER1:	A pointer to the HTTP Server structure where the data will be stored.

RETURN VALUE:	Returns 1 if successful - 0 otherwise.

SEE ALSO: http_parseheader()

END DESCRIPTION **********************************************************/
_http_nodebug int http_parseget( HttpServerStruct *pServer )
{
	// NB: If this returns '1', then server must close the state->spec resource if it
   // is >= 0.  This is because this function now opens the resource.
   auto char *p, *dest, *getdata, *pTopBuffer, *pCurrentBuffer, *pValTop;
   auto unsigned val;
   auto int count;
   auto char cFormState;

   cFormState = FORM_STATE_NONE; // 0 = no form, 1 = has a form, 2 = copy name, 3 = copy value, 4 = done, 5 = error

#ifdef HTTP_VERBOSE
	printf("HTTP: got \"%s\"\n", pServer->inout_buffer);
#endif

   if ( !strncmp("GET ", pServer->inout_buffer, 4) )
   {
      pServer->request_method = HTTP_METHOD_GET;
      p = pServer->inout_buffer + 4;
   }
   else if ( !strncmp("POST ", pServer->inout_buffer, 5) )
   {
      pServer->request_method = HTTP_METHOD_POST;
      p = pServer->inout_buffer + 5;
   }
   else if ( !strncmp("HEAD ", pServer->inout_buffer, 5) )
   {
      pServer->request_method = HTTP_METHOD_HEAD;
      p = pServer->inout_buffer + 5;
   }
   else
   {
      /* unknown command */
      pServer->state = HTTP_DIE;
      return 0;
   }

   /* store the url, and check if there is a form result in it. */
   dest = pServer->request_url; // state->url;

   // while ( (*p != '\0') && (*p != ' ') && ((dest - state->url) < HTTP_MAXURL) )
   while ( (*p != '\0') && (*p != ' ') && ((dest - pServer->request_url)
   			< HTTP_MAXURL) )
   {
      if (*p == '?')
      {
         cFormState = FORM_STATE_HASFORM;
         p++;
			//while ( (*p != '\0') && (*p != ' ') )
		   //		*p++;
			// Remember start of GET data
         getdata = p;
         break;
      }
      else
      {
         *dest = *p;
      }
      p++;
      dest++;
   }

   // Null terminate the URL
   *dest = '\0';     /* null-separate the URL and the form result */

   // Parse the get data into the format necessary for the php interpreter.
   if ( cFormState == FORM_STATE_HASFORM )
   {
   	pTopBuffer = pServer->get_stack_top + sizeof(int);
      pCurrentBuffer = pTopBuffer;
      cFormState = 2;
      count = sizeof(int);
      while ( *getdata != '\0' && count < HTTP_GETPOST_BUFFER )
      {
      	switch ( cFormState )
         {
         case FORM_STATE_COPYNAME:
         	if ( *getdata == '=' )
            {
            	getdata++;

               // Terminate the name string with a null terminator
               *pCurrentBuffer = '\0';

               // Set the length of the name string
               *((int *)(pTopBuffer - sizeof(int))) = (int)(pCurrentBuffer - pTopBuffer);

#ifdef HTTP_VERBOSE
               printf( "[CD] Got Get Pair: Name=\"%s\" Length=%d\n", pTopBuffer, *((int *)(pTopBuffer - sizeof(int))) );
#endif

               // Update the pointer to the proper location
               pCurrentBuffer = pCurrentBuffer + sizeof(int)+1;	// (length) and NULL terminator
               pTopBuffer = pCurrentBuffer;
               count += sizeof(int)+1;

               // Move to the next state
               cFormState = FORM_STATE_COPYVAL;

               // Set the starting address of the value string to the current
               // location.
               pValTop = getdata;
            }
      		else if ( isalpha(*getdata) )
         	{
         		// Copy the name
         		*pCurrentBuffer = *getdata;
            	pCurrentBuffer++;
            	getdata++;
               count++;
         	}
            else if ( pTopBuffer == pCurrentBuffer )
            	cFormState = FORM_STATE_DONE;
            else
            	cFormState = FORM_STATE_ERROR;

            break;

         case FORM_STATE_COPYVAL:
         	if ( *getdata == '&' || *getdata == ' ' || *(getdata + 1) == '\0' ) // Are we at the end of a the getdata?
            {
            	if ( *(getdata + 1) == '\0' )
                  getdata++;

               // Decode the value string
               http_urldecode( pCurrentBuffer, pValTop, ((getdata - pValTop)+1) );

               // Set the length of the value string
               *((int *)(pTopBuffer - sizeof(int))) = (int)strlen( pCurrentBuffer );

#ifdef HTTP_VERBOSE
               printf( "[CD] Got Get Pair: Value=\"%s\" Length=%d\n", pCurrentBuffer, *((int *)(pTopBuffer - sizeof(int))) );
#endif

               // Update the pointers to the proper locations
               pCurrentBuffer += (*((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1);
               count += (*((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1);

               pTopBuffer = pCurrentBuffer;


               // Update the form state
               if ( *getdata == '&' )
               {
                  cFormState = FORM_STATE_COPYNAME;
                  getdata++;
               }
               else
                  cFormState = FORM_STATE_DONE;
            }
            else
            	getdata++;

         	break;

         default:
         	cFormState = FORM_STATE_ERROR;
         	break;
      	}

         if ( cFormState == FORM_STATE_ERROR )
      	{
         	pServer->get_stack_top = pServer->get_stack_end = pServer->get_post_buffer;
         	pServer->get_stack_top[0] = '\0';
            break;
   		}
      	else if ( cFormState == FORM_STATE_DONE )
      	{
      		pServer->get_stack_end = (pTopBuffer - 1);
         	pServer->post_stack_top = pServer->post_stack_end = (pTopBuffer);
            break;
      	}
      }

		p = getdata;
   }

   pServer->http_version = HTTP_VER_09;

   if (*p != '\0')
   {
	   ++p;
	   if (!strncmp(p, "HTTP/1.0", 8))
	      pServer->http_version = HTTP_VER_10;
	   else if (!strncmp(p, "HTTP/1.1", 8))
	      pServer->http_version = HTTP_VER_11;
	}

   return 1;
}



/* START FUNCTION DESCRIPTION ********************************************
http_parsepost                   <HTTP.LIB>

SYNTAX: int http_parsepost(HttpServerStruct *pServer);

KEYWORDS:		tcpip, http, transfer-coding

DESCRIPTION:	Parses the POST data.

PARAMETER1:	A pointer to the HTTP Server structure where the data will be stored.

RETURN VALUE:	Returns 1 if successful - 0 otherwise.

SEE ALSO: http_parseheader()

END DESCRIPTION **********************************************************/
_http_nodebug int http_parsepost( HttpServerStruct *pServer )
{
	// NB: If this returns '1', then server must close the state->spec resource if it
   // is >= 0.  This is because this function now opens the resource.
   auto char *p, *getdata, *pTopBuffer, *pCurrentBuffer, *pValTop;
   auto int count;
   auto int iDataRead;
   auto char *pMaxRead;
   auto char cFormState;

#ifdef HTTP_VERBOSE
	printf("HTTP: parse post \"%s\"\n", pServer->inout_buffer);
#endif

	if ( strcmp( pServer->content_type, "application/x-www-form-urlencoded" ) != 0 )
   {
#ifdef HTTP_VERBOSE
		printf( "HTTP: parse post -- don't know Content-Type!\n" );
#endif

   	// We don't know how to read anything but urlencoded post data for now
   	pServer->state = HTTP_DIE;
   	return 0;
   }

   getdata = p = (pServer->inout_buffer);

   // Parse the get data into the format necessary for the php interpreter.
   pTopBuffer = pServer->post_stack_top + sizeof(int);
   pCurrentBuffer = pTopBuffer;
   cFormState = FORM_STATE_COPYNAME;
   count = sizeof(int);

   pMaxRead = (p + (unsigned int)pServer->content_length);

   while ( getdata <= pMaxRead && count < HTTP_GETPOST_BUFFER )
   {
      switch ( cFormState )
      {
      case FORM_STATE_COPYNAME:
         if ( *getdata == '=' )
         {
            getdata++;

            // Terminate the name string with a null terminator
            *pCurrentBuffer = '\0';

            // Set the length of the name string
            *((int *)(pTopBuffer - sizeof(int))) = (int)(pCurrentBuffer - pTopBuffer);

            #ifdef HTTP_VERBOSE
            printf( "[CD] Got POST Pair: Name=\"%s\" Length=%d\n", pTopBuffer, *((int *)(pTopBuffer - sizeof(int))) );
            #endif

            // Update the pointer to the proper location
            pCurrentBuffer = pCurrentBuffer + sizeof(int)+1;	// (length) and NULL terminator
            pTopBuffer = pCurrentBuffer;
            count += sizeof(int)+1;

            // Move to the next state
            cFormState = FORM_STATE_COPYVAL;

            // Set the starting address of the value string to the current
            // location.
            pValTop = getdata;
         }
         else // if ( isalpha(*getdata) )
         {
            // Copy the name
            *pCurrentBuffer = *getdata;
            pCurrentBuffer++;
            getdata++;
            count++;
         }
         /*
         else if ( pTopBuffer == pCurrentBuffer )
            cFormState = FORM_STATE_DONE;
         else
            cFormState = FORM_STATE_ERROR;
         */
         break;

      case FORM_STATE_COPYVAL:
         if ( *getdata == '&' || getdata >= pMaxRead ) // Are we at the end of a the getdata?
         {
            // Decode the value string
            http_urldecode( pCurrentBuffer, pValTop, ((getdata - pValTop)+1) );

            // Set the length of the value string
            *((int *)(pTopBuffer - sizeof(int))) = (int)strlen( pCurrentBuffer );

            #ifdef HTTP_VERBOSE
            printf( "[CD] Got POST Pair: Value=\"%s\" Length=%d\n", pCurrentBuffer, *((int *)(pTopBuffer - sizeof(int))) );
            #endif

            // Update the pointers to the proper locations
            pCurrentBuffer += (*((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1);
            count += (*((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1);

            pTopBuffer = pCurrentBuffer;


            // Update the form state
            if ( *getdata == '&' )
            {
               cFormState = FORM_STATE_COPYNAME;
               getdata++;
            }
            else
               cFormState = FORM_STATE_DONE;
         }
         else
            getdata++;

         break;

      default:
         cFormState = FORM_STATE_ERROR;
         break;
      }

      if ( cFormState == FORM_STATE_ERROR )
      {
         pServer->post_stack_top[0] = '\0';
         
			return 0;
      }
      else if ( cFormState == FORM_STATE_DONE )
      {
         pServer->post_stack_end = (pTopBuffer - 1);

			return 1;
      }
   }
}

_http_nodebug int http_parsehead(HttpServerStruct *pServer, int part)
{
   auto int temp, count;
   auto char *p, *q, *r;
   auto char *pTopBuffer, *pCurrentBuffer, *pValTop;
   auto char cCookieState;

#ifdef HTTP_VERBOSE
	printf("HTTP: http_parsehead: %shdr \"%s\"\n", part ? "part " : "", pServer->inout_buffer);
#endif

	if ( !part )
   {
	   if ( !strncmpi(pServer->inout_buffer, "Cookie:", 7) )
      {
      	p = pServer->inout_buffer + 8;
         pTopBuffer = pServer->cookie_buffer + sizeof(int);
      	pCurrentBuffer = pTopBuffer;
      	cCookieState = COOKIE_STATE_COPYNAME;
         count = sizeof(int);

      	while ( *p != '\0' && count < HTTP_COOKIE_BUFFER )
      	{
      		switch ( cCookieState )
         	{
         	case COOKIE_STATE_COPYNAME:
         		if ( *p == '=' )
            	{
            		p++;

                  // Null terminate the string
                  *pCurrentBuffer = '\0';

               	// Set the length of the name string
               	*((int *)(pTopBuffer - sizeof(int))) = (int)(pCurrentBuffer - pTopBuffer);

                  #ifdef HTTP_VERBOSE
                  printf( "[CD] Got Cookie: Name=\"%s\" Length=%d\n", pTopBuffer, *((int *)(pTopBuffer - sizeof(int))) );
                  #endif

               	// Update the pointer to the proper location
               	pCurrentBuffer = pCurrentBuffer + sizeof(int)+1;	// (length) and NULL terminator
               	pTopBuffer = pCurrentBuffer;
                  count += sizeof(int)+1; 	// We need room for 3 more elements

               	// Move to the next state
               	cCookieState = COOKIE_STATE_COPYVAL;

               	// Set the starting address of the value string to the current
               	// location.
               	pValTop = p;
            	}
               else if ( *p == ' ' )
               	p++;
      			else if ( *p != ',' && *p != ';' )
         		{
         			// Copy the name
         			*pCurrentBuffer = *p;
            		pCurrentBuffer++;
            		p++;
                  count++; // We have copied an element
         		}
            	else
            		cCookieState = COOKIE_STATE_ERROR;

            	break;

         	case COOKIE_STATE_COPYVAL:
         		if ( *p == ';' || *(p + 1) == '\0' )
            	{
               	if ( *(p + 1) == '\0' )
                     p++;

                  // Decode the value string
                  http_urldecode( pCurrentBuffer, pValTop, ((p - pValTop)+1) );

                  // Set the length of the value string
                  *((int *)(pTopBuffer - sizeof(int))) = (int)strlen( pCurrentBuffer );
                  count += *((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1;

                  pCurrentBuffer += *((int *)(pTopBuffer - sizeof(int))) + sizeof(int)+1;

                  #ifdef HTTP_VERBOSE
                  printf( "[CD] Got Cookie: Value=\"%s\" Length=%d\n", pTopBuffer, *((int *)(pTopBuffer - sizeof(int))) );
                  #endif

                  pTopBuffer = pCurrentBuffer;

                  // Update the form state
                  if ( *p == ';' )
                  {
                     cCookieState = COOKIE_STATE_COPYNAME;
                     p++;
                  }
                  else
                     cCookieState = COOKIE_STATE_DONE;
            	}
               else
               	p++;

         		break;

         	default:
         		cCookieState = COOKIE_STATE_ERROR;
           		break;
      		}

            if ( cCookieState == COOKIE_STATE_ERROR )
      		{
      			pServer->cookie_buffer[0] = '\0';
            	pServer->cookie_stack_end = pServer->cookie_buffer;
               break;
   			}
      		else if ( cCookieState == COOKIE_STATE_DONE )
      		{
      			pServer->cookie_stack_end = (pTopBuffer - 1);
               break;
      		}
      	}



   #ifdef HTTP_VERBOSE
		printf("HTTP: cookie '%s'\n",pServer->inout_buffer);
	#endif
	      return 0;
	   } /* END Cookie */

	   if ( !strncmpi(pServer->inout_buffer, "If-Modified-Since:", 18) )
      {
	      return 0;
	   } /* END If-Modified-Since */
   }

   if ( !strncmpi(pServer->inout_buffer, "Content-Length: ", 16) )
   {
		if (part)
      	pServer->part_content_length = atol( pServer->inout_buffer + 16 );
      else
      	pServer->content_length = atol( pServer->inout_buffer + 16 );
      return 0;
   } /* END Content-Length */

   // Process POST Data one element at a time
	if ( !strncmpi(pServer->inout_buffer, "Content-Disposition: ", 21) )
   {

   	p = pServer->inout_buffer + 21; // Skip past the header
      pServer->tag_name[0] = 0;		  // Set the first character to zero

      if (q = strchr(p, ';')) *q = 0;	// Locate the separator

      if ( !strcmpi(p, "form-data") )
      {
      	pServer->disposition = MIME_DISP_FORMDATA; // Set the disposition type to form data
      	while (q)
         {
            while ( isspace(*++q) );
            if ( !strncmpi(q, "name=", 5) )
            {
            	q += 5;
               if (*q == '"')
               {
               	p = ++q;
                  if (q = strchr(p, '"'))
                  	temp = q - p;	// length of name string
                  else
                  	temp = strlen(p);
               }
               else
               {
               	p = q;
               	if (q = strchr(p, ';'))
                  	temp = q - p;
                  else
                  	temp = strlen(p);
               }

               if (temp > sizeof(pServer->tag_name)-1)
               	temp = sizeof(pServer->tag_name)-1;

               memcpy(pServer->tag_name, p, temp);
               pServer->tag_name[temp] = 0;
               #ifdef HTTP_VERBOSE
               printf("HTTP: form-data name=\"%s\"\n", pServer->tag_name);
               #endif
            }
            else if ( !strncmpi(q, "filename=", 9) )
            {
            	q += 9;
               if (*q == '"')
               {
               	p = ++q;
                  if (q = strchr(p, '"'))
							temp = q - p;
                  else
                  	temp = strlen(p);

               }
               else
               {
               	p = q;
                  if (q = strchr(p, ';'))
                  	temp = q - p;
                  else
                  	temp = strlen(p);
               }
               for (r = p + temp - 1; r > p; --r)
               {
               	if (*r == '\\' || *r == '/')
                  {
                  	temp -= r - p + 1;
							p = r + 1;
                     break;
                  }
               }

               if(temp > sizeof(pServer->tag_filename) - 1)
                  temp = sizeof(pServer->tag_filename) - 1;
               memcpy(pServer->tag_filename, p, temp);
               pServer->tag_filename[temp] = 0;
               #ifdef HTTP_VERBOSE
               printf("HTTP: form-data filename=\"%s\"\n", pServer->tag_filename);
               #endif
            }
            if (q = strchr(q, ';'))
            	*q = 0;	// try next parm
            else
            	break;

         }
      }
      else if (!strcmpi(p, "inline"))
      	pServer->disposition = MIME_DISP_INLINE;
      else if (!strcmpi(p, "attachment"))
      	pServer->disposition = MIME_DISP_ATTACHMENT;
   }

   if ( !strncmpi(pServer->inout_buffer, "Content-Type: ", 14) )
   {
#ifdef HTTP_VERBOSE
		printf( "HTTP: http_parsehead: Parsing Content-Type\n" );
#endif
   	p = pServer->inout_buffer + 14;
		if (q = strchr(p, ';')) *q = 0;
      temp = strlen(p);

      if ( temp > sizeof(pServer->content_type) - 1 ) temp = sizeof( pServer->content_type ) - 1;

      memcpy(pServer->content_type, pServer->inout_buffer + 14, temp);
      pServer->content_type[temp] = '\0';

      if (!part && !strcmpi(pServer->content_type, "multipart/form-data"))
      {
      	while (q)
         {
            while ( isspace(*++q) );
            if ( !strncmpi(q, "boundary=", 9) )
            {
            	q += 9;
               if (*q == '"')
               {
               	p = ++q;
                  if (q = strchr(p, '"'))
                  	temp = q - p;	// length of boundary string
                  else
                  	temp = strlen(p);
               }
               else
               {
               	p = q;
               	if (q = strchr(p, ';'))
                  	temp = q - p;
                  else
                  	temp = strlen(p);
               }

               if (temp > sizeof(pServer->boundary) - 4)
               	temp = sizeof(pServer->boundary) - 4;

               memcpy(pServer->boundary, "\r\n--", 4);
               memcpy(pServer->boundary+4, p, temp);
               pServer->boundary_len = temp+4;
               #ifdef HTTP_VERBOSE
               printf("HTTP: boundary=\"%*.*s\"\n", temp, temp, pServer->boundary+4);
               #endif
               break;
            }
            if (q = strchr(q, ';')) *q = 0;	// try next parm
         }
      }
      return 0;
   } /* END Content-Type */

   return 1;
}

/*
 * This function does the necessary processing when the current
 * connection must be aborted.  It is OK to call more than once.
 */

_http_nodebug void _http_abort(int x)
{
   // JW - Do nothing
}

/* START FUNCTION DESCRIPTION **************************************************
http_init

SYNTAX: int http_init( int (*pHandlerFunction)( ) );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Initialize the http server daemon

RETURN VALUE:  	none

SEE ALSO: 	http_handler

END DESCRIPTION ***************************************************************/

_http_nodebug int http_init( int (*pHandlerFunction)( ) )
{
	auto int iServer;
   auto HttpServerStruct *pServer;
   int sz;

   http_servers = malloc( sizeof(HttpServerStruct) * HTTP_MAXSERVERS );

   for ( iServer = 0; iServer < HTTP_MAXSERVERS; iServer++ )
   {
   	// load_server_from_xmem( ppPhysicalAddressForServers, iServer, pServerBuffer );
   	pServer = &http_servers[iServer];

#if HTTP_VERBOSE
	printf( "HTTP: http_init: Initializing server %d\n", iServer );
#endif

      pServer->state = HTTP_INIT;	// We just finished initialized the server
      pServer->output_length = 0;	// There is no output available

      pServer->input_pos = pServer->inout_buffer;
      pServer->input_length = 0;

      /*
       * JW - Fix the offset of the header position in the last 1/4 of the
       * output buffer
       */
      pServer->output_length = 0;
      pServer->output_offset = 0;
      sz = HTTP_MAX_OUTPUTBUFFER;
      pServer->output_buffer_reserved_loc = malloc( sz ); // _xalloc( &sz, 0, XALLOC_MAYBBB );
      pServer->output_buffer_loc = pServer->output_buffer_reserved_loc;

      pServer->header_length = 0;
      pServer->header_offset = 0;
      sz = HTTP_MAX_HEADERBUFFER;
      pServer->header_buffer_pp = malloc( sz ); // _xalloc( &sz, 0, XALLOC_MAYBBB );

      pServer->error_state = 0; // No errors starting out

      pServer->username[0] = '\0';
      pServer->password[0] = '\0';

      pServer->http_process_handler = pHandlerFunction;

      sz = (PHP_MAX_VARIABLETABLE * PHP_MAX_STRINGLENGTH);
      pServer->php_interpreter.ppStringVarBufLoc = malloc( sz ); // _xalloc( &sz, 0, XALLOC_MAYBBB );
      // save_server_into_xmem( ppPhysicalAddressForServers, iServer, pServerBuffer );
	}




   return 0;
}

/* START FUNCTION DESCRIPTION **************************************************
http_server

SYNTAX: int http_server( void );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Tick function to run the http daemon. Must be called
	periodically to allow the http_server to function properly.

RETURN VALUE:  	none

SEE ALSO: 	http_init

END DESCRIPTION ***************************************************************/

_http_nodebug int http_server( void )
{
   auto int iServer;
   auto int temp;
   auto int len;
   auto HttpServerStruct *pServer;
   // auto tcp_Socket *pSocket;


   // Give the TCP/IP Daemon Life
   // tcp_tick(NULL);

   for ( iServer = 0; iServer < HTTP_MAXSERVERS; iServer++ )
   {
      // Load the server from xmem memory to work with the data in root
      // load_server_from_xmem( ppPhysicalAddressForServers, iServer, pServerBuffer );

   	pServer = &http_servers[iServer];
      // pSocket = &(pServer->s);

      /*
       *	Handle Timeouts
       */
	if( pServer->state != pServer->laststate )
	{
		/* state changed; reset timeout */
		pServer->laststate = pServer->state;
		pServer->main_timeout = set_timeout(HTTP_TIMEOUT);
	}

	if( chk_timeout(pServer->main_timeout) )
	{
		/* TODO: Handle timeout processing better
		if( (pServer->state == HTTP_LISTEN) || (pServer->state == HTTP_INIT) )
		{
					pServer->main_timeout = set_timeout(HTTP_TIMEOUT);
		}
		else
		{
	#ifdef HTTP_VERBOSE
					printf("HTTP: TIMEOUT!\n");
	#endif
					// sock_abort( &pServer->s );
					pServer->state = HTTP_WAITCLOSE;
		}
		*/
	}

#ifdef HTTP_VERBOSE
 	printf("HTTP: socket in state %d\n", pServer->state);
#endif

      switch ( pServer->state )
      {
      case HTTP_INIT:      		// Initialize the HTTP Server
         /*memset((char *)&h->HTTP_FIRST_FIELD_TO_ZERO, 0,
               (char *)&((HttpState *)0)->HTTP_FIRST_FIELD_NOT_TO_ZERO -
               (char *)&((HttpState *)0)->HTTP_FIRST_FIELD_TO_ZERO);
         */

         // http_sock_extlisten(h,HTTP_IFACE,HTTP_PORT,0,0,NULL,0,0,0);
	//		tcp_extlisten( &(pServer->s), HTTP_IFACE, HTTP_PORT, 0, 0, NULL, 0, 0, 0 );

         // Plain HTTP server, do plain HTTP listen
         pServer->state = HTTP_LISTEN;
         break;

      case HTTP_LISTEN:  			// Listen for a connection

         // if ( !sock_waiting( &(pServer->s) ) )
	//		{
         	// Not waiting any more: either established or aborted.  In the latter case,
            // the above sock_alive() test will have failed so we don't need to consider it
            // here.
            // http_sock_mode(h, HTTP_MODE_ASCII);
            // sock_mode( &(pServer->s), TCP_MODE_ASCII );

            pServer->state = HTTP_GETREQ;

            pServer->output_length = 0;	// There is no output available

      		pServer->input_pos = pServer->inout_buffer;
      		pServer->input_length = 0;

        		/*
        		* JW - Reset the offsets and lengths of the output/header buffers
       		*/
            pServer->output_length = 0;
            pServer->output_offset = 0;
            pServer->header_length = 0;
            pServer->header_offset = 0;

            // Set the output buffer to the reserved location of the buffer - this
            // may change if the output buffer points to an xmem file.
            pServer->output_buffer_loc = pServer->output_buffer_reserved_loc;

            /*
            * JW - Set up the get and post buffer
            *
            */
            pServer->get_post_buffer[0] = '\0';
            pServer->get_stack_top = pServer->get_stack_end = pServer->get_post_buffer;
   			pServer->post_stack_top = pServer->post_stack_end = pServer->get_post_buffer;

            /* Setup the cookie buffer */
            pServer->cookie_buffer[0] = '\0';
            pServer->cookie_stack_end = pServer->cookie_buffer;

      		pServer->error_state = 0; // No errors starting out
         // }
         break;

      case HTTP_GETREQ: 			// Get initial request line
         if ( http_getline(pServer) )
         {
#ifdef HTTP_VERBOSE
		printf("HTTP: GETREQ: %s\n", pServer->inout_buffer );
#endif
#ifdef HTTP_VERBOSE
	printf( "HTTP: state HTTP_LISTEN: pServer->header_buffer_pp: %x\n", pServer->header_buffer_pp );
#endif
            if ( !http_parseget(pServer) )
            {
               // http_sock_close(h);
               // sock_close( &(pServer->s) ); // Close the socket
               pServer->state = HTTP_WAITCLOSE;
            }
            else
            {
               pServer->state = HTTP_GETHEAD;
            }
            break;
         }
         break;

      case HTTP_GETHEAD: 			// Get Headers from HTTP Request
         if ( http_getline(pServer) ) // Get a single line from the input
         {
            if ( pServer->inout_buffer[0] == '\0')
            {
            	if ( pServer->request_method == HTTP_METHOD_POST )
               {
               	pServer->state = HTTP_HANDLEPOST;
               }
               else
               {
               	/* did we receive a blank line? if so, end of head */
	            	pServer->state = HTTP_PROCESS;
               }
            }
            else
            {
               http_parsehead(pServer, 0);
            }
         }
         break;

      case HTTP_HANDLEPOST:
			if ( http_getline( pServer ) )
			{
				if ( !http_parsepost(pServer) )
				{
					// http_sock_close(h);
					// sock_close( &(pServer->s) ); // Close the socket
					pServer->state = HTTP_WAITCLOSE;
				}
				else
				{
					pServer->state = HTTP_PROCESS;
				}
			}
         break;


      case HTTP_PROCESS: 			// Process the request
#ifdef HTTP_VERBOSE
	printf( "HTTP: state HTTP_PROCESS: pServer->header_buffer_pp: %x\n", pServer->header_buffer_pp );
#endif
      	if ( pServer->http_process_handler )
         	(*pServer->http_process_handler)( pServer );
         else
         	pServer->state = HTTP_DIE;
         break;

      case HTTP_FINISHWRITE:
#if HTTP_VERBOSE
	printf( "HTTP_FINISHWRITE\n" );
#endif
      	// Finish sending an HTML page by sending out the content data.

         if ( (pServer->output_offset + HTTP_MAX_INOUTBUFFER) > pServer->output_length )
         	len = pServer->output_length - pServer->output_offset; // Send only the remaining data.
         else
         	len = HTTP_MAX_INOUTBUFFER;		// Send up to the size of the buffer.

         // Load the block to be transmitted into the inout_buffer.
         xmem2root( pServer->inout_buffer, (pServer->output_buffer_loc + pServer->output_offset), len );

         // Attempt to send the block up to the maximum size.
	 /*
         if ( (temp = sock_fastwrite( &(pServer->s), pServer->inout_buffer, (int)len )) < 0 )
         {
            // Error encountered while sending data
            pServer->state = HTTP_DIE;
            break;
         }
	 */
	 if ( (temp = write_output( pServer->inout_buffer, len )) < 0 )
	 {
	 	// Error encountered while sending data
		pServer->state = HTTP_DIE;
		break;
	 }

         // Move past the amount of data transmit
         pServer->output_offset += temp;

         /* reset timeout */
         if ( temp )
         	pServer->main_timeout = set_timeout( HTTP_TIMEOUT );

         // Check to see if we are finished transmitting all the data?
         if ( pServer->output_offset >= pServer->output_length )
         {
            // Finished writing out all the data.  Move to the next state
            pServer->state = pServer->nextstate;
         }
         break;
      /*
      	if ( (temp = sock_fastwrite( &(pServer->s),
              pServer->output_content_pos + (int)pServer->output_offset,
              (int)pServer->output_length - (int)pServer->output_offset)) < 0)
         {
				// Error on socket
         	pServer->state = HTTP_DIE;
            break;
         }
         pServer->output_offset += temp;

         // reset timeout
         if( temp )
         	pServer->main_timeout = set_timeout(HTTP_TIMEOUT);

         if ( pServer->output_offset >= pServer->output_length )
         {
         	// Finished writing old data.  Buffer now free.
            pServer->state = pServer->nextstate;
			}
         break;
      */
      case HTTP_SENDPAGE:
#if HTTP_VERBOSE
	printf( "HTTP: HTTP_SENDPAGE: (%d, %d)\n", pServer->header_length, pServer->header_offset );
#endif
      	// Send an html page - start by sending the headers from the header area of the buffer, then finish write.
      	// We have more header data to send - send the amount we are capable of sending

         if ( (pServer->header_offset + HTTP_MAX_INOUTBUFFER) > pServer->header_length )
         	len = pServer->header_length - pServer->header_offset; // Send only the remaining data.
         else
         	len = HTTP_MAX_HEADERBUFFER;		// Send up to the size of the buffer.

         // Load the block to be transmitted into the inout_buffer.
         xmem2root( pServer->inout_buffer, (pServer->header_buffer_pp + pServer->header_offset), len );

         // Attempt to send the block up to the maximum size.
	 /*
         if ( (temp = sock_fastwrite( &(pServer->s), pServer->inout_buffer, (int)len )) < 0 )
         {
          	// Error encountered while sending data
           	pServer->state = HTTP_DIE;
            break;
         }
	 */

#if HTTP_VERBOSE
	printf( "Writing output: %d\n", len );
#endif
	 if ( (temp = write_output( pServer->inout_buffer, len )) < 0 )
	 {
#if HTTP_VERBOSE
	printf( "Write output returned < 0, DIE\n" );
#endif
	 	// Error encountered while sending data
		pServer->state = HTTP_DIE;
		break;
	 }

         // Move past the amount of data transmit
         pServer->header_offset += temp;

         /* reset timeout */
         if ( temp )
         	pServer->main_timeout = set_timeout( HTTP_TIMEOUT );

         // Check to see if we need to change states to send content data?
         if ( pServer->header_offset >= pServer->header_length )
         {
         	// Completed header data transmission move on to content data.
         	pServer->output_offset = 0;	// Next state is transmit output data
            										// set its offset to 0.
            pServer->state = HTTP_FINISHWRITE;
            pServer->nextstate = HTTP_DIE;

#if HTTP_VERBOSE
	printf( "HTTP_SENDPAGE complete going to HTTP_FINISHWRITE\n" );
#endif
         }
         break;

      case HTTP_DIE:
#if HTTP_VERBOSE
	printf( "HTTP_DIE: Going to WAITCLOSE\n" );
#endif

         // sock_close( &(pServer->s) ); // Close the socket
         pServer->state = HTTP_WAITCLOSE;
         // fall through

      case HTTP_WAITCLOSE:
      	// Just to be sure the socket can close...
      	// This is a slight hack, necessitated by the limitations of parsing strings
      	// byte-by-byte.  Sometimes there is a trailing CRLF in the socket which we
      	// have not read.  Must chew it up to proceed.
	/*
         if ( (len = sock_bytesready( &(pServer->s) )) >= 0 )
         {
#ifdef HTTP_VERBOSE
				printf("HTTP: discarded %d bytes at close\n", len);
#endif
				if (len == 0)
            {
					// Must be in ASCII mode with a blank line waiting for us
					// http_sock_gets(h, h->buffer, 2);
               sock_gets( &(pServer->s), pServer->inout_buffer, 2 );
				}
            sock_fastread( &(pServer->s), NULL, (int)len ); // Discard extraneous
      	}
	*/
	 _http_abort( iServer );
	 
	 // Our request close indicator
	 puts( "\r\n" );

	 // Return to listen for next connection
	 pServer->state = HTTP_LISTEN;
         break;

      default: /* doh... how did we get here??? */
               /* reset controller... */
         exit(-1);
         break;
      }

      // Save this server since we are done manipulating its data
      //save_server_into_xmem( ppPhysicalAddressForServers, iServer, pServerBuffer );
   }

   return 0;
}

/* START FUNCTION DESCRIPTION ********************************************
http_flush_header                  		<MY_HTTP.LIB>

SYNTAX: int http_flush_header( HttpServerStruct *pServer, int iLength );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Flushes the header data in the inout_buffer by copying it into
				 the xmem buffer.

PARAMETER1:  The HTTP Server Structure
PARAMETER2:  The length of the data to be flushed.

RETURN VALUE:  The amount flushed if successful, -1 if not.

SEE ALSO: 	http_server

END DESCRIPTION **********************************************************/
int http_flush_header( HttpServerStruct *pServer, int iLength )
{
#if HTTP_VERBOSE
	printf( "HTTP: http_flush_header: Flushing header (%d, %d) %x\n", pServer->header_length, iLength, pServer->header_buffer_pp );
#endif

	if ( root2xmem( (pServer->header_buffer_pp + pServer->header_length), pServer->inout_buffer, iLength ) < 0 )
   		return (-1);

	pServer->header_length += iLength;

#if HTTP_VERBOSE
	printf( "HTTP: http_flush_header: Done flushing header (%d, %d)\n", pServer->header_length, iLength );
#endif
	return ((int)iLength);
}


/* START FUNCTION DESCRIPTION ********************************************
http_write_output                   		<MY_HTTP.LIB>

SYNTAX: int http_write_output( HttpServerStruct *pServer, int iLength );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Flushes the output data in the inout_buffer by copying it into
				 the xmem buffer.

PARAMETER1:  The HTTP Server Structure
PARAMETER2:  The length of the data to be flushed.

RETURN VALUE:  The amount flushed if successful, -1 if not.

SEE ALSO: 	http_server

END DESCRIPTION **********************************************************/
int http_write_output( HttpServerStruct *pServer, char *pString, int iLength )
{
	if ( (iLength + pServer->inout_buffer_length) >= HTTP_MAX_INOUTBUFFER )
	{
#if HTTP_VERBOSE
		printf ( "HTTP: http_write_output: Flushing output!\n" );
#endif
		// Force flush of the inout_buffer to physical memory
		if ( root2xmem( (pServer->output_buffer_reserved_loc + pServer->output_length), pServer->inout_buffer, pServer->inout_buffer_length ) < 0 )
			return (-1);

		pServer->output_length += pServer->inout_buffer_length;
		pServer->inout_buffer_length = 0; // Reset the length in the inout_buffer to zero.
	}

#if HTTP_VERBOSE
	printf( "HTTP: http_write_output: Copying inout_buffer (%d, %d)\n", pServer->inout_buffer_length, iLength );
#endif

	memcpy( (pServer->inout_buffer + pServer->inout_buffer_length), pString, iLength );

	pServer->inout_buffer_length += iLength;

#if HTTP_VERBOSE
	printf( "HTTP: http_write_output: Wrote %d\n", iLength );
#endif

   return (iLength);
}


/* START FUNCTION DESCRIPTION ********************************************
http_flush_inout_buffer                   		<MY_HTTP.LIB>

SYNTAX: int http_flush_inout_buffer( HttpServerStruct *pServer, int iLength );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Flushes the output data in the inout_buffer by copying it into
				 the xmem buffer.

PARAMETER1:  The HTTP Server Structure
PARAMETER2:  The length of the data to be flushed.

RETURN VALUE:  The amount flushed if successful, -1 if not.

SEE ALSO: 	http_server

END DESCRIPTION **********************************************************/
int http_flush_inout_buffer( HttpServerStruct *pServer )
{
	if ( pServer->inout_buffer_length >= 0 )
   {
   	// Force flush of the inout_buffer to physical memory
      if ( root2xmem( (pServer->output_buffer_reserved_loc + pServer->output_length), pServer->inout_buffer, pServer->inout_buffer_length ) < 0 )
   		return (-1);

      pServer->output_length += pServer->inout_buffer_length;
      pServer->inout_buffer_length = 0; // Reset the length in the inout_buffer to zero.
   }

   return (0);
}


/* START FUNCTION DESCRIPTION ********************************************
MoveSlidingWindow                   		<MY_HTTP.LIB>

SYNTAX: int MoveSlidingWindow( HttpServerStruct *pServer, char bForceMove );

KEYWORDS:		tcpip, http

DESCRIPTION: 	Flushes the output data in the inout_buffer by copying it into
				 the xmem buffer.

PARAMETER1:  The HTTP Server Structure
PARAMETER2:  The length of the data to be flushed.

RETURN VALUE:  The amount flushed if successful, -1 if not.

SEE ALSO: 	http_server

END DESCRIPTION **********************************************************/
int MoveSlidingWindow( HttpServerStruct *pServer, char bForceMove )
{
   auto PHPInterpreterStruct *pInterp;

   pInterp = (&pServer->php_interpreter);

   pInterp->pFileData = pInterp->ppFileLocation;

   return 1;
}
