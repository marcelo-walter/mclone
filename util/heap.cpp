/*
 *  heap.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------------------
 
 heap.c
 Routines to manipulate a heap
 
 Based on:
 Algorithms by Robert Sedgewick
 The HeapRemove returns the minimum of the heap (in the book
 the algorithms are for returning the max of the set).
 The comparisons for inserting and deleting elements
 into the heap are made based on the value of "eventTime"
 of the EVENT structure.
 
 Author: Marcelo Walter
 Date: 3/27/95
 
 *---------------------------------------------------------------
 */

#include "heap.h"

#include <stdlib.h>

#include "genericUtil.h"

HEAP *myheap;


/*
 *-----------------------------------------------------------
 *	 Maintains the heap property
 *-----------------------------------------------------------
 */
void HeapUp(HEAP *h, int k)
{
	int v;
	EVENT *a;
	int *p, *q;
	
	a = h->array;
	p = h->p;
	q = h->q;
	v = p[k];
	
	a[p[0]].eventTime= -1e20;
	while(a[p[k/2]].eventTime >= a[v].eventTime){
		p[k] = p[k/2];
		q[p[k/2]] = k;
		k = k/2;
	}
	p[k] = v;
	q[v] = k;
}

/*
 *-------------------------------------------------------------
 *	Insert a new element into the heap
 *-------------------------------------------------------------
 */
void HeapInsert(HEAP *h, EVENT *a)
{
	int size;
	
	if(h->size == h->max) HeapGrow( h, HEAP_GROW_FACTOR);
	
	size = ++(h->size);
	h->array[size].whichEvent = a->whichEvent;
	h->array[size].eventTime = a->eventTime;
	h->array[size].info = a->info;
	
	h->p[size] = size;
	h->q[size] = size;
	
	HeapUp(h,size);
	
}

/*
 *-------------------------------------------------------------------
 *	Since the heap is maintained in an array structure, the necessary
 *	space is allocated in "blocks" of HEAP_GROW_FACTOR size.
 *--------------------------------------------------------------------
 */
void HeapGrow(HEAP *h, int grow)
{
	h->max += grow;
	if((h->array = (HEAP_ELEMENT *) realloc(h->array, (h->max+2) * sizeof (HEAP_ELEMENT)))==NULL)
		errorMsg("Problem with realloc in heap.c!");
	
	if((h->p = (int *) realloc(h->p, (h->max+2) * sizeof (int)))==NULL)
		errorMsg("Problem with realloc in heap.c!");
	
	if((h->q = (int *) realloc(h->q, (h->max+2) * sizeof (int)))==NULL)
		errorMsg("Problem with realloc in heap.c!");
	
}


/*
 *-----------------------------------------------------------
 *	Inits the heap->head structure and returns a pointer
 *	to the heap.
 *-----------------------------------------------------------
 */
HEAP *HeapInit(int size)
{
	
	HEAP *h;
	
	if((h = (HEAP *) malloc(sizeof(HEAP)))==NULL)
		errorMsg("Problem with malloc in HeapInit!");
	
	h->size = 0;
	h->max = size;
	if((h->array = (HEAP_ELEMENT *) malloc( (size+2) * sizeof( HEAP_ELEMENT)))==NULL)
		errorMsg("Problem with malloc in HeapInit!");
	
	if((h->p = (int *) malloc( (size+2) * sizeof(int)))==NULL)
		errorMsg("Problem with malloc in HeapInit!");
	
	if((h->q = (int *) malloc( (size+2) * sizeof(int)))==NULL)
		errorMsg("Problem with malloc in HeapInit!");
	
	/* Addition to the code of Jim Boritz */
	h->p[0] = 0;
	h->q[0] = 0;
	
	return(h);
}

/*
 *-----------------------------------------------------
 *
 *----------------------------------------------------
 */
void HeapPrint(HEAP *h)
{
	int i;
	
	printf("ele val ord que\n");
	for( i=1; i<= h->size; i++)
		printf("%3d %3f %9d %3d\n", i, h->array[i].eventTime ,h->p[i], h->q[i]);
}

/*
 *-----------------------------------------------------
 *
 *----------------------------------------------------
 */
void HeapOrderPrint1(HEAP *h)
{
	int i,size;
	EVENT *a;
	
	size = h->size;
	printf("Sorted heap\n");
	for(i=1; i <= size; i++){
		a = HeapRemove(h);
		switch(a->whichEvent){
			case SPLIT:
				printf( "%d %f cell(%3.3f %3.3f)\n",
					   a->whichEvent, a->eventTime, a->info->x, a->info->y );
				break;
			case RELAX:
				printf( "%d %f\n", a->whichEvent, a->eventTime );
				break;
			default:
				break;
		}
	}
}

/*
 *--------------------------------------------------------
 *
 *-------------------------------------------------------
 */
void EmptyHeap(HEAP *h)
{
	int i,size;
	EVENT *a;
	
	size = h->size;
	printf("Please wait...Emptying the heap\n");
	for(i=1; i <= size; i++) a = HeapRemove(h);
}

/*
 *---------------------------------------------------------
 * Free the space used by the Heap and starts a new heap
 *---------------------------------------------------------
 */
HEAP *ClearHeap(HEAP *h)
{
	
	HEAP *nh;
	
	free(h->array); h->array=NULL;
	free(h->p); h->p=NULL;
	free(h->q); h->q=NULL;
	free(h); h=NULL;
	nh = HeapInit(HEAP_GROW_FACTOR);
	return(nh);
	
}

/*
 *---------------------------------------------------------
 *    Removes an element from the heap
 *---------------------------------------------------------
 */
EVENT *HeapRemove(HEAP *h)
{
	EVENT *a,*v;
	int *p,*q;
	int size;
	
	a = h->array;
	p = h->p;
	q = h->q;
	size = h->size;
	
	if( size == 0) errorMsg("Empty Heap !");
	
	if((v = (HEAP_ELEMENT *) malloc(sizeof(HEAP_ELEMENT)))==NULL)
		errorMsg("Problem with malloc in HeapRemove!");
	
	v->whichEvent = a[p[1]].whichEvent;
	v->eventTime = a[p[1]].eventTime;
	v->info = a[p[1]].info;
	
	a[p[1]].whichEvent = a[size].whichEvent;
	a[p[1]].eventTime = a[size].eventTime;
	a[p[1]].info = a[size].info;
	
	q[p[1]] = q[size];
	p[q[size]] = p[1];
	q[p[size]] = 1;
	p[1] = p[size];
	
	h->size --;
	HeapDown(h,1);
	
	return(v);
	
}

/*
 *--------------------------------------------------
 *	The same function as HeapUp
 *--------------------------------------------------
 */
void HeapDown(HEAP *h, int k)
{
	
	int j,v;
	int size;
	HEAP_ELEMENT *a;
	int *p, *q;
	
	a = h->array;
	p = h->p;
	q = h->q;
	size = h->size;
	
	v = p[k];
	while( k <= size/2){
		j = k+k;
		if (j < size && a[p[j]].eventTime > a[p[j+1]].eventTime) j++;
		if (a[v].eventTime <= a[p[j]].eventTime) break;
		p[k] = p[j]; q[p[j]] = k; k = j;
	}
	p[k] = v; q[v] = k;
    
}





