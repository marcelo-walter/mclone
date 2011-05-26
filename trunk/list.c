/*
 *  list.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *-------------------------------------------------------------------
 *
 * LIST.C
 *
 * Code for manipulating the cells linked list
 * This code is NOT totally implementation-independent
 * the routines are written for the kind of data
 * they are manipulating. There are two types of list
 * manipulation routines in this code, one for what I
 * call 'Full cell' CELL and another one for a simpler
 * cell data structure called SCELL
 * 
 * Marcelo Walter
 *
 *-------------------------------------------------------------------
 */

/*
 *-------------------------------------------------------------------
 *   INCLUDES
 *-------------------------------------------------------------------
 */
#include <stdlib.h>
#include "common.h"

/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
/* head and tail nodes of the global linked list */
int cellSize = sizeof(CELL);
int cellSize_S = sizeof(SCELL);

/*
 *-----------------------------------------------------------
 * Free the space used by the global linked list of cells
 *-----------------------------------------------------------
 */
void freeListOfCells( CELL *h, CELL *t )
{
	CELL *s, *to_remove;
	
	s = h->next;
	while(s != t){
		to_remove = s;
		s = to_remove->next;
		removeFromListOfCells(to_remove, h);
	}
}
/*
 *-----------------------------------------------------------
 * Same as above for SIMPLE CELL
 *-----------------------------------------------------------
 */
void freeListOfCells_S( SCELL *h, SCELL *t )
{
	SCELL *s, *to_remove;
	
	s = h->next;
	while(s != t){
		to_remove = s;
		s = to_remove->next;
		removeFromListOfCells_S(to_remove, h);
	}
}

/*
 *-----------------------------------------------------------
 * Allocate space for a new node and returns
 * a pointer to the newly allocated node.
 * If malloc cannot find space the routine
 * exits.
 *-----------------------------------------------------------
 */
CELL *cellAlloc(void)
{
	CELL *n;
	
	if((n = (CELL *) malloc( cellSize )) == NULL)
		errorMsg("Error with malloc in function cellAlloc()!");
	
	n->next = (CELL *)NULL;
	return n;
}
/*
 *-----------------------------------------------------------
 *	Same as above for SIMPLE CELLS
 *-----------------------------------------------------------
 */
SCELL *cellAlloc_S(void)
{
	SCELL *n;
	
	if((n = (SCELL *) malloc( cellSize_S )) == NULL)
		errorMsg("Error with malloc in function cellAlloc_S!");
	n->originalCell = (CELL *)NULL;
	n->next = (SCELL *)NULL;
	return n;
}
/*
 *-----------------------------------------------------------
 *  Inserts a node in the list
 *  Receives a pointer to the node to be
 *  inserted, a pointer to the head
 *  node of the list and to the
 *  tail node
 *-----------------------------------------------------------
 */
void insertOnListOfCells(CELL *p, CELL *h, CELL *t)
{
	CELL *s;
	
	s = h;
	while(s->next != t) s = s->next;
	
	s->next = p;
	p->next = t;
}	
/*
 *-----------------------------------------------------------
 *  Inserts a node in the list
 *  Receives a pointer to the node to be
 *  inserted, a pointer to the head
 *  node of the list and to the
 *  tail node
 *-----------------------------------------------------------
 */
void insertOnListOfCells_S(SCELL *p, SCELL *h, SCELL *t)
{
	SCELL *s;
	
	s = h;
	while(s->next != t) s = s->next;
	
	s->next = p;
	p->next = t;
}	
/*
 *-----------------------------------------------------------
 *  Prints the list. Receives a pointer
 *  to the head and tail nodes of
 *  the list
 *-----------------------------------------------------------
 */
void printListOfCells(CELL *h, CELL *t)
{
	CELL *s;
	int i;
	
	s = h->next;
	i = 0;
	while (s != t){
		printf("u: %3.2f v: %3.2f w: %3.2f\n", s->bary.u, s->bary.v, s->bary.w );
		
		printf("[prim %d] s: %3.2f t: %3.2f ",
			   s->prim1, s->p1.u, s->p1.v );
		if ( s->prim2 != FALSE )
			printf("[prim %d] s: %3.2f t: %3.2f\n",
				   s->prim2, s->p2.u, s->p2.v );
		else printf("\n");
		
		printf("x = %3.2f y = %3.2f z = %3.2f type = %d split = %f pol = %d\n\n",
			   s->x, s->y, s->z, s->ctype, s->tsplit, s->whichFace
			   /*,faces[s->whichFace].vorFacesList[i].border*/ );
		s = s->next;
		i++;
	}
}

/*
 *-----------------------------------------------------------
 *  Prints the list. Receives a pointer
 *  to the head and tail nodes of
 *  the list
 *-----------------------------------------------------------
 */
void printListOfCells_S(SCELL *h, SCELL *t)
{
	SCELL *s;
	int i;
	
	s = h->next;
	while (s != t){
		printf("x = %3.2f y = %3.2f toProcess = %d\n", s->p.x, s->p.y, s->toProcess );
		s = s->next;
	}
}
/*
 *-----------------------------------------------------------
 *  Removes a node from the list.
 *  Receives a pointer to the head of the
 *  list and to the node to be removed
 *-----------------------------------------------------------
 */
void removeFromListOfCells(CELL *p, CELL *h)
{
	CELL *s;
	
	s = h;
	while(s->next != p) s = s->next;
	
	s->next = p->next;
	free(p); p = NULL;
}
/*
 *-----------------------------------------------------------
 *  Removes a node from the list.
 *  Receives a pointer to the head of the
 *  list and to the node to be removed
 *-----------------------------------------------------------
 */
void removeFromListOfCells_S(SCELL *p, SCELL *h)
{
	SCELL *s;
	
	s = h;
	while(s->next != p) s = s->next;
	
	s->next = p->next;
	free(p); p = NULL;
}
/*
 *-----------------------------------------------------------
 *-----------------------------------------------------------
 */
void changeFacesList(CELL *p, int newFace, int oldFace)
{
	
	CELL *s, *n;
	
	/* update count of cells for the 2 faces */
	faces[oldFace].ncells--;
	faces[oldFace].nCellsType[p->ctype]--;
	faces[newFace].ncells++;
	faces[newFace].nCellsType[p->ctype]++;
	
	/* remove from old face */
	s = faces[oldFace].head;
	while(s->next != p) s = s->next;
	
	s->next = p->next;
	
	/* include into the new face */
	n = faces[newFace].head;
	while(n->next != faces[newFace].tail) n = n->next;
	
	n->next = p;
	p->next = faces[newFace].tail;
}



