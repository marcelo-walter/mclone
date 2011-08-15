/*
 *  heap.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------------------
 *	heap.h
 *---------------------------------------------------------------
 */

#ifndef _HEAP_H_
#define _HEAP_H_

#include "../data/Types.h"

/* how much to increase the heap each time */
#define HEAP_GROW_FACTOR 	100

typedef enum{SPLIT,SWITCH,RELAX} EVENT_TYPE;

/* Holds an event structure */
typedef struct event{
	EVENT_TYPE whichEvent;
	float eventTime;
	CELL *info;
}EVENT;

typedef EVENT HEAP_ELEMENT;

typedef struct heap{
	HEAP_ELEMENT *array;
	int   *p;
	int   *q;
	int   size;
	int   max;
}HEAP;

extern HEAP *myheap;

/* HEAP.C */
void HeapUp(HEAP *h, int k);
void HeapInsert(HEAP *h, EVENT *a);
void HeapGrow(HEAP *h, int grow);
HEAP *HeapInit(int size);
void HeapPrint(HEAP *h);
void HeapOrderPrint1(HEAP *h);
EVENT *HeapRemove(HEAP *h);
void HeapDown(HEAP *h, int k);
void EmptyHeap(HEAP *h);
HEAP *ClearHeap(HEAP *h);

#endif //_HEAP_H_
