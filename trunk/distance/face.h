#ifndef FACE_H
#define FACE_H

#include "halfedge.h"
#include "vector.h" 

class HalfEdge;

class FaceDistance
	{
	private:
		HalfEdge* he;
		Vector Normal;
		
	public:
		FaceDistance(HalfEdge* HE); 
		FaceDistance(); 
		~FaceDistance();
		
		HalfEdge* getHalfEdge();
		void setHalfEdge(HalfEdge*);
		void Draw(bool,bool,double);
		void ComputeNormal();
		void getNormal(double*,double*,double*);
		void setNormal(Vector);
	};

typedef FaceDistance* pFace;

#endif
