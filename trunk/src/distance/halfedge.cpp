
#include "halfedge.h"

HalfEdge::HalfEdge()
{
	E = NULL;
	V = NULL;
	F = NULL;
	partner = NULL;
	next = NULL;
}

HalfEdge::HalfEdge(EdgeDistance* e, VertexDistance* v, FaceDistance* f, HalfEdge* p, HalfEdge* n)
{
	E = e;
	V = v;
	F = f;
	partner = p;
	next = n;
}

HalfEdge::HalfEdge(EdgeDistance* e, VertexDistance* v, FaceDistance* f)
{
	E = e;
	V = v;
	F = f;
	partner = NULL;
	next = NULL;
}

HalfEdge::HalfEdge(EdgeDistance* e, VertexDistance* v)
{
	E = e;
	V = v;
	F = NULL;
	partner = NULL;
	next = NULL;
}

HalfEdge::~HalfEdge()
{
}

EdgeDistance* HalfEdge::edge()
{
	return E;
}

VertexDistance* HalfEdge::vertex()
{
	return V;
}

FaceDistance* HalfEdge::face()
{
	return F;
}

HalfEdge* HalfEdge::Partner()
{
	return partner;
}

HalfEdge* HalfEdge::Next()
{
	return next;
}

void HalfEdge::setFace(FaceDistance* f)
{
	F = f;
}

void HalfEdge::setPartner(HalfEdge* p)
{
	partner = p;
}

void HalfEdge::setNext(HalfEdge* p)
{
	next = p;
}

// void HalfEdge::setVertex(VertexDistance* v)
// {
//     V = v;
// }
// 
// void HalfEdge::setEdge(EdgeDistance* e)
// {
//     E = e;
// }
