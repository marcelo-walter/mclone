#ifndef VERTEX_H
#define VERTEX_H

#include "halfedge.h"
#include "heapstruct.h"

class HalfEdge;

using namespace std;

class VertexDistance: public MxHeapable{
private:
    double x, y, z;
    HalfEdge *he;
    int pos;
    double *Color;
    double *Normal;
    
public:
	VertexDistance();
	VertexDistance(double, double, double, int);
	VertexDistance(double, double, double, int, HalfEdge * HE);
	
    ~VertexDistance();
	
    double X();
    double Y();
    double Z();
    HalfEdge *getHalfEdge();
    bool operator== (const VertexDistance &);
    int Pos();
    void setHalfEdge (HalfEdge *);
    void setColor(double, double, double);
    void getColor(double *, double *, double *);
    void setNormal(double, double, double);
    void getNormal(double *, double *, double *);
};

typedef VertexDistance *pVertex;

#endif
