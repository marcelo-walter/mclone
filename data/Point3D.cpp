/*
 *------------------------------------------------------------
 *		myvect.c
 *		Marcelo Walter
 *		may, 96
 *
 *		A limited 3D vector library
 *------------------------------------------------------------
 */

#include "Point3D.h"

#include <math.h>



/* return vector difference c = a-b */
Point3D *V3Sub(Point3D *a, Point3D *b, Point3D *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	return(c);
}

/* return the cross product c = a cross b */
Point3D *V3Cross(Point3D *a, Point3D *b, Point3D *c)
{
	c->x = (a->y*b->z) - (a->z*b->y);
	c->y = (a->z*b->x) - (a->x*b->z);
	c->z = (a->x*b->y) - (a->y*b->x);
	return(c);
}

/* returns squared length of input vector */	
double V3SquaredLength(Point3D *a) 
{
	return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
}

/* returns length of input vector */
double V3Length(Point3D *a) 
{
	return(sqrt(V3SquaredLength(a)));
}

/* zero the input vector and returns it */
Point3D *V3Zero(Point3D *v) 
{
	v->x = 0.0;
	v->y = 0.0;
	v->z = 0.0;
	return(v);
}

/* divides the input vector by divisor and returns it */
Point3D *V3Divide(Point3D *v,  double divisor) 
{
	v->x = v->x/divisor;
	v->y = v->y/divisor;
	v->z = v->z/divisor;
	return(v);
}

/* multiplies the input vector by mult and returns it */
Point3D *V3Multiply(Point3D *v,  double mult) 
{
	v->x = v->x * mult;
	v->y = v->y * mult;
	v->z = v->z * mult;
	return(v);
}

/* add vectors u and v into u */
Point3D *V3AddPlus(Point3D *u,  Point3D *v) 
{
	u->x += v->x;
	u->y += v->y;
	u->z += v->z;
	return(u);
}

/* initialize a vector */
Point3D *V3Init(Point3D *v, double value)
{
	v->x = value;
	v->y = value;
	v->z = value;
	return(v);
	
}

/* assign source vector component-wise into destiny vector */
void V3Assign(Point3D *destiny, Point3D source)
{
	destiny->x = source.x;
	destiny->y = source.y;
	destiny->z = source.z;
}

/* return the dot product of vectors a and b */
double V3Dot(Point3D *a, Point3D *b) 
{
	return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
}

/* return the vector a normalized */
Point3D *V3Normalize(Point3D *a)
{
	return(V3Divide( a, V3Length(a)));
}

