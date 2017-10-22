#include "out_helper.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


char *strncat( char *pDest, char *pSource, int len )
{
	if ( len == 0 )
		return (pDest);

	char *pDestCur = pDest;
	char *pSourceCur = pSource;

	while ( *pDestCur != '\0' )
		pDestCur++;

	do
	{
		if ( (*pDestCur = *pSourceCur++) == '\0' )
			break;

		pDestCur++;
	} while ( --len > 0 );

	*pDestCur = '\0';
	
	return (pDest);
}

int xgetlong( void )
{
	// TODO: Fix this
	return (4);
}

int write_output( char *pData, int len )
{
	int dataSent = 0;
	long pos = 0;

	while ( pos < len )
	{
		putc( pData[pos] );

		pos++;
		dataSent++;
	}

	return (dataSent);
}

int xmem2root( char *pDest, char *pSource, int len )
{
	memcpy( pDest, pSource, len );
}

int NonCaseSensitiveValue( int c )
{
	if ( c >= 'A' && c <= 'Z' )
		return ('a' + (c - 'A'));
	else
		return (c);
}

int strncmpi( char *pStr1, char *pStr2, int len )
{
	int idx = 0;
	if ( len == 0 )
		return (0);

	for ( idx = 0; idx < len; idx++ )
	{
		if ( *pStr1 == '\0' || *pStr2 == '\0' )
			break;

		int CompareValue = NonCaseSensitiveValue( *pStr1 ) - NonCaseSensitiveValue( *pStr2 );

		if ( CompareValue != 0 )
			return (CompareValue);

		pStr1++;
		pStr2++;

	}

	if ( idx == len )
		return (0);

	return ( NonCaseSensitiveValue( *pStr1 ) - NonCaseSensitiveValue( *pStr2 ) );
}

int strcmpi( char *pStr1, char *pStr2 )
{
	do
	{
		if ( *pStr1 == '\0' || *pStr2 == '\0' )
			break;

		int CompareValue = NonCaseSensitiveValue( *pStr1 ) - NonCaseSensitiveValue( *pStr2 );

		if ( CompareValue != 0 )
			return (CompareValue);

		pStr1++;
		pStr2++;

	} while ( 1 );

	return ( NonCaseSensitiveValue( *pStr1 ) - NonCaseSensitiveValue( *pStr2 ) );
}

int set_timeout( int msecs )
{
	return (msecs);
}

int chk_timeout( int timeout )
{
	struct timespec tm1;
	struct timespec tm2;

	tm1.tv_sec = 0;
	tm1.tv_msec = timeout;

	millisleep( &tm1, &tm2 );
 
	return (1);
}

int my_pow( int x, int y )
{
	int result;
	while ( y )
	{
		if ( y & 1 )
			result *= x;

		y >>= 1;
	
		x *= x;
	}

	return (result);
}

int root2xmem( char *pDest, char *pSource, int len )
{
	memcpy( pDest, pSource, len );

	return (len);
}
