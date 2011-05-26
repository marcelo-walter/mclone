#include "vertex.h"

#include <iostream>

#include "vector.h"

VertexDistance::VertexDistance()
{
	x = y = z = 0.0;
	he = NULL;
	pos = 0;
	Color = NULL;
	Normal = NULL;
}

VertexDistance::VertexDistance(double X, double Y, double Z, int p, HalfEdge* HE)
{
	x = X;
	y = Y;
	z = Z;
	he = HE;
	pos = p;
	Color = NULL;
	Normal = NULL;
}

VertexDistance::VertexDistance(double X, double Y, double Z, int p)
{
	x = X;
	y = Y;
	z = Z;
	he = NULL;
	pos = p;
	Color = NULL;
	Normal = NULL;
}


VertexDistance::~VertexDistance()
{
	if (Color != NULL)
		delete[] Color;
	if (Normal != NULL)
		delete[] Normal;
}


double VertexDistance::X()
{
	return x;      
}

double VertexDistance::Y() 
{
	return y;
}

double VertexDistance::Z()                                                
{                                                                 
	return z;                                                        
}            

HalfEdge* VertexDistance::getHalfEdge()
{
	return he;
}

bool VertexDistance::operator==(const VertexDistance& V)
{
	return ( (V.x == x) && (V.y == y) && (V.z == z) );
}

int VertexDistance::Pos()
{
	return pos;
}

void VertexDistance::setHalfEdge(HalfEdge* h)
{
	he = h;
}

void VertexDistance::setColor(double r, double g, double b)
{
	Color = new double[3];
	Color[0] = r;
	Color[1] = g;
	Color[2] = b;
}

void VertexDistance::getColor(double* r, double* g, double* b)
{	
	if (Color != NULL)
	{
		*r = Color[0];
		*g = Color[1];
		*b = Color[2];
	}
	else
	{
		cout << "error: tempting to get the color of an ";
		cout << "uncolored vertex." << endl;
		*r = 1.0; // Definir el color rojo por defecto
		*g = *b = 0.0; 
	}
}

void VertexDistance::setNormal(double x, double y, double z)
{
	Vector n = {x,y,z};
	n = (1.0/n.norm())*n;
	
	Normal = new double[3];
	Normal[0] = n.x;
	Normal[1] = n.y;
	Normal[2] = n.z;
}

void VertexDistance::getNormal(double* x, double* y, double* z)
{	
	if (Normal != NULL)
	{
		*x = Normal[0];
		*y = Normal[1];
		*z = Normal[2];
	}
	else
	{
		cout << "error: tempting to get the normal of a ";
		cout << "vertex with no normal defined." << endl;
		*x = 1.0; // Definir normal (1, 0, 0) por defecto
		*y = *z = 0.0; 
	}
}
