
#include "vector.h"
Vector operator+(const Vector& v1, const Vector& v2)
{
	//Vector* V = new Vector;
	Vector V;
	V.x = v1.x+v2.x;
	V.y = v1.y+v2.y;
	V.z = v1.z+v2.z;
	return V;
}


Vector operator-(const Vector& v1, const Vector& v2)
{
	//Vector* V = new Vector;
	Vector V;
	V.x = v1.x-v2.x;
	V.y = v1.y-v2.y;
	V.z = v1.z-v2.z;
	return V;
}

void operator+=(Vector& v1, const Vector& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

double operator*(const Vector& v1, const Vector& v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector operator*(double alfa, const Vector& v2)
{
	//Vector* V = new Vector;
	Vector V;
	V.x = alfa*v2.x;
	V.y = alfa*v2.y;
	V.z = alfa*v2.z;
	return V;
}

Vector operator-(const point& p1, const point& p2)
{
	// Vector* V = new Vector;
	Vector V;
    V.x = p1.x-p2.x;
    V.y = p1.y-p2.y;
    V.z = p1.z-p2.z;
    return V;
	
}

point operator+(const point& p, const Vector& v)
{
 	//point* P = new point;
	point P;
	P.x = p.x+v.x;
	P.y = p.y+v.y;
	P.z = p.z+v.z;
	return P;
	
}

Vector cross(Vector v1, Vector v2)
{
	//Vector* V = new Vector;
	Vector V;
	V.x = v1.y*v2.z - v1.z*v2.y;
	V.y = v1.z*v2.x - v1.x*v2.z;
	V.z = v1.x*v2.y - v1.y*v2.x;
	return V; 
}
