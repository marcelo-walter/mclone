#ifndef HALF_EDGE_H
#define HALF_EDGE_H

#include <stdlib.h>

#include "vertex.h"
#include "face.h"
#include "edge.h"

class EdgeDistance;
class VertexDistance;
class FaceDistance;

class HalfEdge
	{
	private:
		EdgeDistance* E;
		VertexDistance* V;
		FaceDistance* F;
		HalfEdge* partner;
		HalfEdge* next;
		
	public:  
		HalfEdge();
		HalfEdge(EdgeDistance* e, VertexDistance* v);
		HalfEdge(EdgeDistance* e, VertexDistance* v, FaceDistance* f);
		HalfEdge(EdgeDistance* e, VertexDistance* v, FaceDistance* f, HalfEdge* p, HalfEdge* n);
		~HalfEdge();
		
		EdgeDistance* edge();
		VertexDistance* vertex();
		FaceDistance* face();
		HalfEdge* Partner();
		HalfEdge* Next();
		void setFace(FaceDistance*);
		void setPartner(HalfEdge*);
		void setNext(HalfEdge*);
		//void setVertex(VertexDistance*);
		//void setEdge(EdgeDistance*);
	};

#endif
