#ifndef __OUT_HELPER_H__
#define __OUT_HELPER_H__

int write_output( char *pData, int dataLen );

void xmem2root( char *pDest, char *pSource, int len );

int xgetlong( void );

char *strncat( char *pDest, char *pSource, int len );

int strncmpi( char *pStr1, char *pStr2, int len );

int strncmpi( char *pStr1, char *pStr2 );

int set_timeout( int msecs );

int chk_timeout( int timeout );

int my_pow( int x, int y );

int root2xmem( char *pDest, char *pSource, int len );

#endif // __OUT_HELPER_H__
