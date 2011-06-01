/*
 *------------------------------------------------------------
 *		myvect.h
 *		Marcelo Walter
 *		may, 96
 *
 *		A limited 3D vector library
 *------------------------------------------------------------
 */

#ifndef _MY_VECT_H_
#define _MY_VECT_H_

#include "../common.h"

double V3SquaredLength(Point3D *a);
double V3Length(Point3D *a);
Point3D *V3Sub(Point3D *a, Point3D *b, Point3D *c);
Point3D *V3Cross(Point3D *a, Point3D *b, Point3D *c);
void loadIdentity(Matrix4 *m);


#endif //_MY_VECT_H_
