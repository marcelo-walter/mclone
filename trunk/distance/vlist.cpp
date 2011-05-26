#include "vlist.h"

vList::vList()
{
	head = NULL;
	end = NULL;
}

vList::~vList()
{
	clear();
}

void vList::insert(VertexDistance* v, vNode* pos = NULL)
{
	
	if (pos != NULL)
	{
		vNode* vn = new vNode;
		vn->next = pos->next;
		pos->next = vn;
		vn->v = v;
		if ( pos == end)
			end = pos;		
	}
	else if(head == NULL)
	{
		vNode* vn = new vNode;
		vn->next = NULL;
		vn->v = v;
		head = end = vn;
	}
	else
		insert(v, end);				
}

void vList::clear()
{
	vNode* aux;
	while (head != NULL)
	{
		aux = head;
		head = head->next;
		delete aux;		
	}
	
}

vIt* vList::makeIt(vNode* vn = NULL)
{
	return new vIt(this, vn);
}

void vList::destroyIt(vIt* vi)
{
	delete vi;
}



///////////// Iterator ...

vIt::vIt(vList* l, vNode* vn = NULL)
{
	L = l;
	curr = (vn != NULL) ? vn : L->head;
}

vIt::~vIt()
{
}

vNode* vIt::next()
{
	curr = (curr != NULL) ? curr->next : NULL;
	return curr;
}

vNode* vIt::operator++()
{
	return next();
}

vNode* vIt::current()
{
	return curr;
}
