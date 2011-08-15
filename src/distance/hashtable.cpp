#include "hashtable.h"
#include <iostream>

HashTable::HashTable(unsigned int size)
{
	hash_size = size;
	hash_data = new EdgeList[hash_size];
}

HashTable::~HashTable()
{
	delete [] hash_data;
}

unsigned int HashTable::Key(EdgeDistance* E)
{
	HalfEdge** h = E->getHalfEdges();  
	//double val = h[0]->vertex()->X() + h[1]->vertex()->X() +
	//          10.0*( h[0]->vertex()->Y() + h[1]->vertex()->Y() +
	//   		 10.0*( h[0]->vertex()->Z() + h[1]->vertex()->Z()));
	
	int i =  h[0]->vertex()->Pos();
	int j =  h[1]->vertex()->Pos();
	
	if (i < j)
	{
		int aux = i;
		i = j;
		j = aux;
	}
	
	unsigned int K = i*(i-1)/2+j;
	
	return K % hash_size;
}

EdgeDistance* HashTable::insert(EdgeDistance* E, FaceDistance* f)
{
	unsigned int hash_pos = Key(E); 
	EdgeList* EL = &(hash_data[hash_pos]);
	EdgeDistance* e = EL->has(E);
	if (e != NULL)
	{
		// actualiza *e
		if ( e->getHalfEdges()[0]->vertex() == E->getHalfEdges()[0]->vertex() )
		{
            if (e->getHalfEdges()[0]->face() == NULL)
				e->getHalfEdges()[0]->setFace( E->getHalfEdges()[0]->face());
            else
				cout << "Warning: Non oriented Surface" << endl;
		}
		else
		{
            if (e->getHalfEdges()[1]->face() == NULL)
				e->getHalfEdges()[1]->setFace(E->getHalfEdges()[0]->face());
            else
				cout << "Warning: Non oriented Surface" << endl;
		}
		
	}
	else
	{
		EL->insert(E);
		HalfEdge** hes = E->getHalfEdges();
		if (hes[0]->vertex()->getHalfEdge() == NULL)
            hes[0]->vertex()->setHalfEdge(hes[0]);
		if (hes[1]->vertex()->getHalfEdge() == NULL)
            hes[1]->vertex()->setHalfEdge(hes[1]);
		e = E;
		
	}
	return e;
}


void HashTable::CleanEdges()
{
	EdgeList* el;
	for (unsigned int i = 0; i < hash_size; i++)
	{
		el = &(hash_data[i]);
      	
		EdgeNode* en = el->head;
		while (en != NULL)
		{
            EdgeDistance* e = en->edge();
            HalfEdge** hes = e->getHalfEdges();
            if (hes[0]-> face() == NULL)
            {
            	// clean hes[0]
            	// 1. Chequear que el vertice no apunte
            	//    para hes[0]
				if (hes[0]->vertex()->getHalfEdge() == hes[0]) 
					hes[0]->vertex()->setHalfEdge(hes[1]->Next());
            	
            	// 2. Borrar hes[0]
				delete hes[1]->Partner();
            	
            	// 3. Actualizar partner
				hes[1]->setPartner(NULL);
            }	
            if (hes[1]-> face() == NULL)
            {
            	// clean hes[1]
            	// 1. Chequear que el vertice no apunte
            	//    para hes[1]
				if (hes[1]->vertex()->getHalfEdge() == hes[1]) 
					hes[1]->vertex()->setHalfEdge(hes[0]->Next());
            	
            	// 2. Borrar hes[0]
            	
				delete hes[0]->Partner();
				
            	// 3. Actualizar partner
				hes[0]->setPartner(NULL);		
            }
         	en = en->Next();	
		}
	}
}

