/*
 *  cellsList.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *-------------------------------------------------------------------
 *
 * cellsList.h
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

#ifndef _CELLS_LIST_H_
#define _CELLS_LIST_H_


#include "../common.h"

/*
 *---------------------------------------------------------------
 *
 * VARIABLES
 *
 *---------------------------------------------------------------
 */
/* keep track of how many cells of each type we have */
extern int nCellsType[HOW_MANY_TYPES];

extern flag cellsCreated;
extern flag pickRandomFlag;

/* head and tail nodes of the global linked list */
extern int cellSize;
extern int cellSize_S;


void createRandomCells( int quantity, int embryoShape );
void freeListOfCells( CELL *h, CELL *t );
void freeListOfCells_S( SCELL *h, SCELL *t );
CELL *cellAlloc(void);
SCELL *cellAlloc_S(void);
void insertOnListOfCells(CELL *p, CELL *h, CELL *t);
void insertOnListOfCells_S(SCELL *p, SCELL *h, SCELL *t);
void printListOfCells(CELL *h, CELL *t);
void printListOfCells_S(SCELL *h, SCELL *t);
void removeFromListOfCells(CELL *p, CELL *h);
void removeFromListOfCells_S(SCELL *p, SCELL *h);
void changeFacesList(CELL *p, int newFace, int oldFace);
// From genericUtil.c
void saveCellsFile( const char *fileName );
int loadCellsFile( const char *inpFileName );


#endif //_CELLS_LIST_H_
