#ifndef VECTOR
#define VECTOR

#include <math.h>

struct Vector
{
	double x;
	double y;
	double z;
	double norm(){return sqrt(x*x+y*y+z*z);};
};

struct point
{
	double x;
	double y;
	double z;
};

Vector operator+(const Vector& v1, const Vector& v2);
Vector operator-(const Vector& v1, const Vector& v2);
void operator+=(Vector& v1, const Vector& v2);
double operator*(const Vector& v1, const Vector& v2);
Vector operator*(double alfa, const Vector& v2);

Vector operator-(const point& p1, const point& p2);
point operator+(const point& p, const Vector& v);

Vector cross(Vector,Vector);

#endif
