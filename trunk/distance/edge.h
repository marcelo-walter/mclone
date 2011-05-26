#ifndef EDGE_H
#define EDGE_H

#include "halfedge.h"

class VertexDistance;
class FaceDistance;

class EdgeDistance
	{
		friend class HalfEdge;
		
	private:
		HalfEdge* he1;
		HalfEdge* he2;
		
	public:
		EdgeDistance(VertexDistance* v1, VertexDistance* v2, FaceDistance* f);
		EdgeDistance(VertexDistance* v1, VertexDistance* v2);
		~EdgeDistance();
		
		HalfEdge** getHalfEdges();
		bool operator==(const EdgeDistance& E);
	};

#endif
