/*
 *  heapTri.h
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

#ifndef _HEAP_TRI_H_
#define _HEAP_TRI_H_

#include "../common.h"

/*
 *-----------------------------------------------------------
 *	 VARIABLES
 *-----------------------------------------------------------
 */
extern HEAPTRI *myheapTri;

/*
 *-----------------------------------------------------------
 *	 PROTOTYPES
 *-----------------------------------------------------------
 */
void HeapUpTri(HEAPTRI *h, int k);
void HeapInsertTri(HEAPTRI *h, HEAPTRI_ELEMENT *a);
void HeapGrowTri(HEAPTRI *h, int grow);
HEAPTRI *HeapInitTri(int size);
void HeapPrintTri(HEAPTRI *h);
void HeapOrderPrint1Tri(HEAPTRI *h);
void EmptyHeapTri(HEAPTRI *h);
HEAPTRI *ClearHeapTri(HEAPTRI *h);
HEAPTRI_ELEMENT *HeapRemoveTri(HEAPTRI *h);
void HeapDownTri(HEAPTRI *h, int k);
HEAPTRI_ELEMENT *allocateHeapTriElem( void );


#endif //_HEAP_TRI_H_
