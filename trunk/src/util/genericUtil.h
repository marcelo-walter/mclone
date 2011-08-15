/*
 *-----------------------------------------------------------
 *	genericUtil.h
 *-----------------------------------------------------------
 */

#ifndef _GENERIC_UTIL_H_
#define _GENERIC_UTIL_H_

#include <stdio.h>
#include "../data/Types.h"

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
int **allocate2DArrayInt( int r, int c );
void beep (void);
float computeGlobalScale( int index, float max );

int Getline(char s[], int lim, FILE *fp);
void toOpenGL(Matrix4 m, float *glm);
void switchFlag(byte *flag);
void minMax(Point3D vert, Point3D *max, Point3D *min);
void assignValue(float v[], float value);
void multiplyValue(float v[], float value);
void assignValueVector(float v[], float value[XYZ]);
float deg2rad(float angle);
float rad2deg(float angle);
double distance( Point3D p, Point3D q );

void errorMsg( char error_text[] );

//From splineInter.c
void linint(float xa[], float ya[], int n, float x, float *y);


#endif //_GENERIC_UTIL_H_
