#ifndef EDGELIST_H
#define EDGELIST_H

#include "edge.h"
class EdgeNode
	{
	private:
		EdgeDistance* e;
		EdgeNode* next;
	public:
		EdgeNode(EdgeDistance* E, EdgeNode* N);
		EdgeNode(EdgeDistance* E);
		~EdgeNode();
		
		EdgeNode* Next();      
		EdgeDistance* edge();
	};

class EdgeList
	{
	public:
		EdgeNode* head;
		
	public:
		EdgeList();
		~EdgeList();
		
		void insert(EdgeDistance*);
		EdgeDistance* has(EdgeDistance*);
	};

#endif
