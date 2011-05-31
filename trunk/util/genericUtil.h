/*
 *-----------------------------------------------------------
 *	genericUtil.h
 *-----------------------------------------------------------
 */

#ifndef _GENERIC_UTIL_H_
#define _GENERIC_UTIL_H_

/* for the parsing of the growth data file */
#define	DATA	"data"
#define	SIZE	"size"
#define	COMMENT	"#"
#define	MONTH	0

/* local prototypes */
int *myIntMalloc( int quantity );
void readExpFile( const char *fileName );
float **allocate2DArrayFloat( int r, int c );
char **allocate2DArrayChar( int r, int c );
byte **allocate2DArrayByte( int r, int c );

//From splineInter.c
void linint(float xa[], float ya[], int n, float x, float *y);


#endif //_GENERIC_UTIL_H_
