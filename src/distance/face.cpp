#include "face.h"

//#include <GL/gl.h>
#include "my_opengl.h"

#include "vector.h"

#include <iostream>

FaceDistance::FaceDistance(HalfEdge* HE)
{
	he = HE;
	Normal.x = Normal.y = Normal.z = 0.0;
}

FaceDistance::FaceDistance()
{
	he = NULL;
	Normal.x = Normal.y = Normal.z = 0.0;
}

FaceDistance::~FaceDistance()
{
}

HalfEdge* FaceDistance::getHalfEdge()
{
	return he;
}

void FaceDistance::setHalfEdge(HalfEdge* h)
{
	he = h;     
}

void FaceDistance::Draw (bool col, bool dist, double M)
{
    if (dist) {
		glBegin (GL_POLYGON);
		double H = -he->vertex ()->heap_key (); 
		if (H > M) 
			glColor3f(0.7,0.7,0.7);
		else 
			glColorHSV3f (40.0 + 320.0 * H / M, 1.0, 1.0); 
		
		double x, y, z;
		he->vertex ()->getNormal (&x, &y, &z);
		glNormal3f (x, y, z);
		glVertex3f (he->vertex ()->X (), he->vertex ()->Y (),
					he->vertex ()->Z ()); 
		for (HalfEdge * h = he->Next (); h != he; h = h->Next ()) {
			H = -h->vertex ()->heap_key ();
			if (H > M)
				glColor3f(0.7,0.7,0.7);
			else
				glColorHSV3f (40.0 + 320.0 * H / M, 1.0, 1.0);
			h->vertex ()->getNormal (&x, &y, &z);
			glNormal3f (x, y, z);
			glVertex3f (h->vertex ()->X (), h->vertex ()->Y (),
						h->vertex ()->Z ());
		}
		glEnd ();
		
    } else if (col) {
		glBegin (GL_POLYGON);
		double r, g, b;
		he->vertex ()->getColor (&r, &g, &b);
		glColor3f (r, g, b);
		he->vertex ()->getNormal (&r, &g, &b);
		glNormal3f (r, g, b);
		glVertex3f (he->vertex ()->X (), he->vertex ()->Y (),
					he->vertex ()->Z ());
		for (HalfEdge * h = he->Next (); h != he; h = h->Next ()) {
			h->vertex ()->getColor (&r, &g, &b);
			glColor3f (r, g, b);
			h->vertex ()->getNormal (&r, &g, &b);
			glNormal3f (r, g, b);
			glVertex3f (h->vertex ()->X (), h->vertex ()->Y (),
						h->vertex ()->Z ());
		}
		glEnd ();
    } else {
		glBegin (GL_POLYGON);
		double x, y, z;
		he->vertex ()->getNormal (&x, &y, &z);
		glNormal3f (x, y, z);
		glVertex3f (he->vertex ()->X (), he->vertex ()->Y (),
					he->vertex ()->Z ());
		for (HalfEdge * h = he->Next (); h != he; h = h->Next ()) {
			h->vertex ()->getNormal (&x, &y, &z);
			glNormal3f (x, y, z);
			glVertex3f (h->vertex ()->X (), h->vertex ()->Y (),
						h->vertex ()->Z ());
		}
		glEnd ();
    }
	
}

void FaceDistance::ComputeNormal()
{
	VertexDistance* V1 = he->vertex();
	VertexDistance* V2 = he->Next()->vertex();
	VertexDistance* V3 = he->Next()->Next()->vertex();
	
	point p1 = {V1->X(), V1->Y(), V1->Z()};
	point p2 = {V2->X(), V2->Y(), V2->Z()};
	point p3 = {V3->X(), V3->Y(), V3->Z()};
	
	Vector n = cross(p2-p1,p3-p2);
	Normal = n;
}

void FaceDistance::getNormal(double* x, double* y, double* z)
{	
	*x = Normal.x;
	*y = Normal.y;
	*z = Normal.z;
}

void FaceDistance::setNormal(Vector V)
{	
	Normal.x = V.x;
	Normal.y = V.y;
	Normal.z = V.z;
}
