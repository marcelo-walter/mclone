/*
 *------------------------------------------------------------
 *		myvect.h
 *		Marcelo Walter
 *		may, 96
 *
 *		A limited 3D vector library
 *------------------------------------------------------------
 */

#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include "Point3D.h"

#include "Types.h"

/*
typedef struct Matrix4Struct {	// 4-by-4 matrix
	double element[4][4];
}Matrix4;*/


#define DAMN_SMALL (1e-30)
#define L_EPSILON 1.0e-10
#define SMALL (1.0e-12)

void MatMul(Matrix4 *a, Matrix4 *b, Matrix4 *c);
void MatMulCum(Matrix4 *a, Matrix4 *b);
void V3PosMul(Point3D *p, Matrix4 *m);
void V3PreMul(Point3D *p, Matrix4 *m);
void loadIdentity(Matrix4 *m);
void checkIdentity( Matrix4 *a );
void MatrixCopy( Matrix4 *source, Matrix4 *destiny);
void MatRotateZ(double theta, Matrix4 *m);
int MxRotateAxis(Point3D p1, Point3D p2, double theta, Matrix4 *TM, Matrix4 *iTM);
double MxInvert( Matrix4 *a, Matrix4 *b);
double MxDeterminant ( Matrix4 *a, int order );
static double MxElementMinor(Matrix4 *a, register int i, register int j, int order);
void MxTranspose( Matrix4 *a, Matrix4 *r);
void MxZero ( Matrix4 *a );
void copyMatrix( Matrix4 *destiny, Matrix4 source );


#endif //_MATRIX4_H_
