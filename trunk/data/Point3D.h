/*
 *------------------------------------------------------------
 *		myvect.h
 *		Marcelo Walter
 *		may, 96
 *
 *		A limited 3D vector library
 *------------------------------------------------------------
 */

#ifndef _POINT3D_H_
#define _POINT3D_H_

#include "../common.h"

/*typedef struct point3d{
	double x,y,z;
}Point3D;*/

double V3SquaredLength(Point3D *a);
double V3Length(Point3D *a);
Point3D *V3Sub(Point3D *a, Point3D *b, Point3D *c);
Point3D *V3Cross(Point3D *a, Point3D *b, Point3D *c);
Point3D *V3Zero(Point3D *v);
Point3D *V3Divide(Point3D *v,  double divisor);
Point3D *V3Multiply(Point3D *v,  double mult);
Point3D *V3AddPlus(Point3D *u,  Point3D *v);
Point3D *V3Init(Point3D *v, double value);
Point3D *V3Normalize(Point3D *a);
void V3Assign(Point3D *destiny, Point3D source);
double V3Dot(Point3D *a, Point3D *b);


#endif //_POINT3D_H_
