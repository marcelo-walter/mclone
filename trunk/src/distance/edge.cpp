#include "edge.h"

EdgeDistance::EdgeDistance(VertexDistance* v1, VertexDistance* v2, FaceDistance* f)
{
	he1 = new HalfEdge(this, v1, f);
	he2 = new HalfEdge(this, v2);
	he1->setPartner(he2);
	he2->setPartner(he1);
}

EdgeDistance::EdgeDistance(VertexDistance* v1, VertexDistance* v2)
{
	he1 = new HalfEdge(this, v1, NULL);
	he2 = new HalfEdge(this, v2);
	he1->setPartner(he2);
	he2->setPartner(he1);
}

EdgeDistance::~EdgeDistance()
{
	if ( he1 != NULL) delete he1;
	if ( he2 != NULL) delete he2;
} 

HalfEdge** EdgeDistance::getHalfEdges()
{
	HalfEdge** he = new HalfEdge*[2];
	he[0] = he1;
	he[1] = he2;
	return he;
}

bool EdgeDistance::operator==(const EdgeDistance& E)
{ 
	return ( (*(he1->vertex()) == *(E.he1->vertex()) &&  *(he2->vertex()) == *(E.he2->vertex())) || 
            (*(he1->vertex()) == *(E.he2->vertex()) &&  *(he2->vertex()) == *(E.he1->vertex())) ); 
}



