#include <iostream>
#include "geolist.h"

geoList::geoList ()
{
    head = NULL;
    end = NULL;
}

geoList::~geoList ()
{
    clear();
}

void geoList::insert(VertexDistance * V1, VertexDistance * V2, double par, geoNode * pos)
{
    if (pos != NULL) 
	{
		geoNode *g = new geoNode;
		g->next = pos->next;
		g->prev = pos;
		if (pos->next != NULL)
			pos->next->prev = g;
		pos->next = g;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		if (pos == end)
			end = pos;
    } else if (head == NULL) 
	{
		geoNode *g = new geoNode;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		g->next = NULL;
		g->prev = NULL;
		head = end = g;
	} else {
		geoNode *g = new geoNode;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		g->next = NULL;
		g->prev = end;
		end->next = g;
		end = g;
	}
}

void geoList::insert(VertexDistance * V1)
{
	VertexDistance * V2 = NULL;
	double par = 1.0;
	geoNode * pos = NULL;
	
    if (pos != NULL) 
	{
		geoNode *g = new geoNode;
		g->next = pos->next;
		g->prev = pos;
		if (pos->next != NULL)
			pos->next->prev = g;
		pos->next = g;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		if (pos == end)
			end = pos;
	} else if (head == NULL)
	{
		geoNode *g = new geoNode;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		g->next = NULL;
		g->prev = NULL;
		head = end = g;
	} else {
		geoNode *g = new geoNode;
		g->V1 = V1;
		g->V2 = V2;
		g->t = par;
		g->next = NULL;
		g->prev = end;
		end->next = g;
		end = g;
	}
}

void geoList::clear()
{
    geoNode *aux;
    while (head != NULL)
	{
		aux = head;
		head = head->next;
		delete aux;
    }
}

geoIt *geoList::makeIt(geoNode * gn)
{
    return new geoIt(this, gn);
}

geoIt *geoList::makeIt()
{
    return new geoIt (this, NULL);
}

void geoList::destroyIt(geoIt * gi)
{
    delete gi;
}

void geoList::remove(geoNode * pos)
{
    if (pos == head) 
	{
		if (end == head)
			head = end = NULL;
		else {
			head = head->next;
			head->prev = NULL;
		}
		delete pos;
		return;
    }
	
    geoNode *aux = pos->prev;
    if (aux != NULL) 
	{
		if (pos == end) 
		{
			end = aux;
			aux->next = NULL;
		} else {
			aux->next = pos->next;
			pos->next->prev = aux;
		}
		delete pos;
    }
}


///////////// Iterator ...

geoIt::geoIt(geoList * l, geoNode * gn = NULL)
{
    L = l;
    curr = (gn != NULL) ? gn : L->head;
}

geoIt::~geoIt ()
{
}

void geoIt::next ()
{
    curr = (curr != NULL) ? curr->next : NULL;
}

void geoIt::operator++ ()
{
    return next ();
}

geoNode *geoIt::data ()
{
    return curr;
}

bool geoIt::more_elem ()
{
    return curr != NULL;
}

void geoIt::prev ()
{
    curr = (curr != NULL) ? curr->prev : NULL;
}

void geoIt::operator-- ()
{
    return prev ();
}
