/*
 *  heapTri.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */



/*
 *---------------------------------------------------------------
 
 heapTri.c
 Routines to manipulate a heap SPECIFICALLY FOR A LIST
 OF TRIANGLES AT EACH VERTEX
 
 Based on:
 Algorithms by Robert Sedgewick
 The HeapRemove returns the minimum of the heap (in the book
 the algorithms are for returning the max of the set).
 The comparisons for inserting and deleting elements
 into the heap are made based on the value of "eventTime"
 of the EVENT structure.
 
 Author: Marcelo Walter
 Date: 3/17/98
 
 *---------------------------------------------------------------
 */

#include "heapTri.h"


extern void copyMatrix( Matrix4 *destiny, Matrix4 source );

/*
 *-----------------------------------------------------------
 *	 Maintains the heap property
 *-----------------------------------------------------------
 */
void HeapUpTri(HEAPTRI *h, int k)
{
	int v;
	HEAPTRI_ELEMENT *a;
	int *p, *q;
	
	a = h->array;
	p = h->p;
	q = h->q;
	v = p[k];
	
	a[p[0]].distance = -1e20;
	while(a[p[k/2]].distance >= a[v].distance){
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
void HeapInsertTri(HEAPTRI *h, HEAPTRI_ELEMENT *a)
{
	int size;
	
	if(h->size == h->max) HeapGrowTri( h, HEAPTRI_GROW_FACTOR);
	
	size = ++(h->size);
	h->array[size].faceIndex = a->faceIndex;
	h->array[size].edgeIndex = a->edgeIndex;
	h->array[size].vertIndex = a->vertIndex;
	h->array[size].distance = a->distance;
	copyMatrix( &(h->array[size].t), a->t );
    
	h->p[size] = size;
	h->q[size] = size;
	
	HeapUpTri( h, size );
	
}

/*
 *-------------------------------------------------------------------
 *	Since the heap is maintained in an array structure, the necessary
 *	space is allocated in "blocks" of HEAP_GROW_FACTOR size.
 *--------------------------------------------------------------------
 */
void HeapGrowTri(HEAPTRI *h, int grow)
{
	h->max += grow;
	if((h->array = (HEAPTRI_ELEMENT *)
		realloc(h->array, (h->max+2) * sizeof (HEAPTRI_ELEMENT)))==NULL)
		errorMsg("Problem with realloc in heapTri.c! (array)");
	
	if((h->p = (int *) realloc(h->p, (h->max+2) * sizeof (int)))==NULL)
		errorMsg("Problem with realloc in heapTri.c! (p)");
	
	if((h->q = (int *) realloc(h->q, (h->max+2) * sizeof (int)))==NULL)
		errorMsg("Problem with realloc in heapTri.c! (q)");
	
}


/*
 *-----------------------------------------------------------
 *	Inits the heap->head structure and returns a pointer
 *	to the heap.
 *-----------------------------------------------------------
 */
HEAPTRI *HeapInitTri(int size)
{
	
	HEAPTRI *h;
	int i;
	
	if((h = (HEAPTRI *) malloc(sizeof(HEAPTRI)))==NULL)
		errorMsg("Problem with malloc in HeapInit (heapTri.c)!");
	
	h->size = 0;
	h->max = size;
	if((h->array = (HEAPTRI_ELEMENT *) malloc( (size+2) * sizeof( HEAPTRI_ELEMENT)))==NULL)
		errorMsg("Problem with malloc in HeapInit (heapTri.c)!");
	
	if((h->p = (int *) malloc( (size+2) * sizeof(int)))==NULL)
		errorMsg("Problem with malloc in HeapInit (heapTri.c) !");
	
	if((h->q = (int *) malloc( (size+2) * sizeof(int)))==NULL)
		errorMsg("Problem with malloc in HeapInit (heapTri.c)!");
	
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
void HeapPrintTri(HEAPTRI *h)
{
	int i;
	
	printf("ele val ord que\n");
	for( i=1; i<= h->size; i++)
		printf("%3d %3f %9d %3d\n", i, h->array[i].distance, h->p[i], h->q[i]);
}

/*
 *-----------------------------------------------------
 *
 *----------------------------------------------------
 */
void HeapOrderPrint1Tri(HEAPTRI *h)
{
	int i, size;
	HEAPTRI_ELEMENT *a;
	
	size = h->size;
	printf("Sorted heap Tri\n");
	for(i=1; i <= size; i++){
		a = HeapRemoveTri(h);
		printf( "%d %d %d %f \n" ,a->faceIndex, a->edgeIndex, a->vertIndex, a->distance );
	}
}

/*
 *--------------------------------------------------------
 *
 *-------------------------------------------------------
 */
void EmptyHeapTri(HEAPTRI *h)
{
	int i,size;
	HEAPTRI_ELEMENT *a;
	
	size = h->size;
	printf("Please wait...Emptying the heap for triangles\n");
	for(i=1; i <= size; i++) a = HeapRemoveTri(h);
}

/*
 *---------------------------------------------------------
 * Free the space used by the Heap and starts a new heap
 *---------------------------------------------------------
 */
HEAPTRI *ClearHeapTri(HEAPTRI *h)
{
	
	HEAPTRI *nh;
	int i;
	
	/* This is the addition to make sure that we
     freed all requested space */
	free(h->array); h->array=NULL;
	free(h->p); h->p=NULL;
	free(h->q); h->q=NULL;
	free(h); h=NULL;
	nh = HeapInitTri(HEAPTRI_GROW_FACTOR);
	return(nh);
	
}

/*
 *---------------------------------------------------------
 *    Removes an element from the heap
 *---------------------------------------------------------
 */
HEAPTRI_ELEMENT *HeapRemoveTri(HEAPTRI *h)
{
	HEAPTRI_ELEMENT *a,*v;
	int *p,*q;
	int size;
	
	a = h->array;
	p = h->p;
	q = h->q;
	size = h->size;
	
	if( size == 0) errorMsg("Empty Heap for Triangles!");
	
	if((v = (HEAPTRI_ELEMENT *) malloc(sizeof(HEAPTRI_ELEMENT)))==NULL)
		errorMsg("Problem with malloc in HeapRemove (heapTri.c)!");
	
	v->faceIndex= a[p[1]].faceIndex;
	v->edgeIndex= a[p[1]].edgeIndex;
	v->vertIndex= a[p[1]].vertIndex;
	v->distance= a[p[1]].distance;
	copyMatrix( &(v->t), a[p[1]].t );
	
	a[p[1]].faceIndex = a[size].faceIndex;
	a[p[1]].edgeIndex = a[size].edgeIndex;
	a[p[1]].vertIndex = a[size].vertIndex;
	a[p[1]].distance = a[size].distance;
	copyMatrix( &(a[p[1]].t), a[size].t );
	
	
	q[p[1]] = q[size];
	p[q[size]] = p[1];
	q[p[size]] = 1;
	p[1] = p[size];
	
	h->size--;
	HeapDownTri(h,1);
	
	return(v);
	
}

/*
 *--------------------------------------------------
 *	The same function as HeapUp
 *--------------------------------------------------
 */
void HeapDownTri(HEAPTRI *h, int k)
{
	
	int j,v;
	int size;
	HEAPTRI_ELEMENT *a;
	int *p, *q;
	
	a = h->array;
	p = h->p;
	q = h->q;
	size = h->size;
	
	v = p[k];
	while( k <= size/2){
		j = k+k;
		if (j < size && a[p[j]].distance > a[p[j+1]].distance) j++;
		if (a[v].distance <= a[p[j]].distance) break;
		p[k] = p[j]; q[p[j]] = k; k = j;
	}
	p[k] = v; q[v] = k;
    
}

/*
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */
HEAPTRI_ELEMENT *allocateHeapTriElem( void )
{
	HEAPTRI_ELEMENT *a;
	
	if ((a = (HEAPTRI_ELEMENT *) malloc(sizeof(HEAPTRI_ELEMENT))) == NULL)
		errorMsg("Problem with malloc in allocateHeapTriElem (forces.c)!");
	else return a;
}


