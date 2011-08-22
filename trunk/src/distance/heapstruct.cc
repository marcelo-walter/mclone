/************************************************************************
 
 Heap data structure
 
 Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
 
 $Id: MxHeap.cxx,v 1.6 1998/10/26 21:09:01 garland Exp $
 
 ************************************************************************/

#include "a48.h"

////////////////////////////////////////////////////////////////////////
//
// Internal functions for manipulating the heap
//


inline void MxHeap::place(MxHeapable *x, unsigned int i)
{
    ref(i) = x;
    x->set_heap_pos(i);
}

void MxHeap::swap(unsigned int i, unsigned int j)
{
    MxHeapable *tmp = ref(i);
	
    place(ref(j), i);
    place(tmp, j);
}

//Organize the heap with block[0] have the biggest
//value and block[last] have the smalest value os key
void MxHeap::upheap(unsigned int i)
{
    MxHeapable *moving = ref(i);
    uint index = i;
    uint p = parent(i); //Parent return (i-1)/2 but with unsign int. I guess that is return the index before actual.
	
	//moving->heap_key() return the value of the field import.
	//ref(p)->heap_key() return the value of the filed import indicate by the reference os p.
	// if the key of atual is bigger than the second one, then ...
    while( index>0 && moving->heap_key() > ref(p)->heap_key() )
    {
		place(ref(p), index); //Make block[index] pointer to block[p], and block[p] pointer to block. Get with the biggest import
		index = p;
		p = parent(p);
    }
	
    if( index != i )
		place(moving, index);
}

void MxHeap::downheap(unsigned int i)
{
    MxHeapable *moving = ref(i); //Return the pointer from block with index i.
    uint index = i;
    uint l = left(i);
    uint r = right(i);
    uint largest;
	
    while( l<length() )
    {
		if( r<length() && ref(l)->heap_key() < ref(r)->heap_key() )
			largest = r;
		else 
			largest = l;
		
		if( moving->heap_key() < ref(largest)->heap_key() )
		{
			place(ref(largest), index);
			index = largest;
			l = left(index);
			r = right(index);
		}
		else
			break;
    }
	
    if( index != i )
		place(moving, index);
}

////////////////////////////////////////////////////////////////////////
//
// Exported interface to the heap
//

void MxHeap::insert(MxHeapable *t, float v)
{
    if( t->is_in_heap() ) //Verify if is a free nodo.
	{
		return;
	}
	
    t->heap_key(v); //Set the field import of the nodo on the heap.
	
    unsigned int i = add(t); //Verify is have free space and then add one position "logicaly", returning your index.
    t->set_heap_pos(i); //Put the position on the block on the field token.
	
    upheap(i);
}

void MxHeap::update(MxHeapable *t, float v)
{
    SanityCheck( t->is_in_heap() );
    t->heap_key(v);
	
    unsigned int i = t->get_heap_pos();
	
    if( i>0 && v>ref(parent(i))->heap_key() )
		upheap(i);
    else
		downheap(i);
}

MxHeapable *MxHeap::extract()
{
    if( length() < 1 ) //Verify if the field fill is smaler than 1.
	{
		return NULL;
	}
	
    swap(0, length()-1); //Change the position between block[0] and block[fill-1].
    MxHeapable *dead=drop(); //Get the pointer of the last block from the heap and put on the dead.
	
    downheap(0);
    dead->not_in_heap();
    return dead;
}

MxHeapable *MxHeap::remove(MxHeapable *t)
{
    if( !t->is_in_heap() ) return NULL;
	
    int i = t->get_heap_pos();
    swap(i, length()-1);
    drop();
    t->not_in_heap();
	
    if( ref(i)->heap_key() < t->heap_key() )
		downheap(i);
	else upheap(i);
	
    return t;
}

void MxHeap::clear()
{
    while (extract() != NULL)
		;
}

void MxHeap::p(MxHeapable *t)
{
	uint ii, filled, NN;
	MxHeapable *Aux;
	
	filled = length();
	NN = total_space();
	
	for (ii = 0; ii < filled; ii++)
	{
		Aux = raw(ii);
		printf("%d %d [%d] {%f %d}\n", ii, (long int)Aux, (long int)t, Aux->heap_key(), Aux->get_heap_pos());
	}
	
	printf("\n");
	
	for (ii = filled; ii < NN; ii++)
	{
		Aux = raw(ii);
		if (Aux != NULL)
		{
			printf("%d %d [%d] {%f %d}\n", ii, (long int)Aux, (long int)t, Aux->heap_key(), Aux->get_heap_pos());
		}
		else {
			printf("%d %d [%d] {XX XX}\n", ii, (long int)Aux, (long int)t);
		}
	}
	
	printf("%d %d [%d] - [%d] [%d]\n---//---\n", NN, (long int)raw(NN), (long int)t, filled, NN);
}
