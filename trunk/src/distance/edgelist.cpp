#include "edgelist.h"

EdgeNode::EdgeNode(EdgeDistance* E, EdgeNode* N)
{
	e = E;
	next = N;
}

EdgeNode::EdgeNode(EdgeDistance* E)
{
	e = E;
	next = NULL;
}

EdgeNode::~EdgeNode()
{
	if (e != NULL) delete e;
	if (next != NULL) delete next;
}

EdgeNode* EdgeNode::Next()
{
	return next;
}

EdgeDistance* EdgeNode::edge()
{
	return e;
}

EdgeList::EdgeList()
{
	head = NULL;
}

EdgeList::~EdgeList()
{
	if (head != NULL) delete head;
}

void EdgeList::insert(EdgeDistance* e)
{
	head = new EdgeNode(e,head);   
}

EdgeDistance* EdgeList::has(EdgeDistance* e)
{
	EdgeDistance* E = NULL;
	EdgeNode* aux = head;
	while( aux != NULL )
	{
		if ( *(aux->edge()) == *e)
		{
			E = aux->edge();
			aux = NULL; // finalizar el ciclo!!
		}
		else
			aux = aux->Next();
	}
	
	return E;
}

