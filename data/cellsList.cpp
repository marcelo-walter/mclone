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

#include "cellsList.h"

#include <stdlib.h>
#include <string.h>

#include "cells.h"
#include "Object.h"
#include "fileManager.h"
#include "../util/random.h"
#include "../util/genericUtil.h"
#include "../util/distPoints.h"
#include "../simulation/relax.h"
#include "../simulation/simulation.h"
#include "../control/primitives.h"
#include "../control/texture.h"


/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
/* keep track of how many cells of each type we have */
int nCellsType[HOW_MANY_TYPES];

int NumberCells = 0; //From Globals.h
int Seed = -11; //From Globals.h

/* head and tail nodes of the global linked list */
int cellSize = sizeof(CELL);
int cellSize_S = sizeof(SCELL);

flag cellsCreated = FALSE;
flag pickRandomFlag = TRUE;


/*---------------------------------------------------------------
 *	Allocates space for cells and create them
 *	The 'embryoShape' flags if these cells are
 *	being created in the original model (embryoShape == FALSE)
 *	or if they are being created in the embryonic shape
 *---------------------------------------------------------------
 */
void createRandomCells( int quantity, int embryoShape )
{
	//printf("\n quantity: %d (cellsList.c) \n", quantity);
	float randomPolygon;
	float s, t;
	int whichFace;
	int i, j, count=0;
	int prim1, prim2, textID;
	double u, v;
	ColChar textValue;
	Point3D p;
	Point2D q;
	CELL *c;
	CELL_TYPE type;

	for(i=0; i < quantity ; i++)
	{
		randomPolygon = ran1( &Seed );
		//printf("\nrandomPolygon: %f, ", randomPolygon);
		/* returns an index for which face we are connecting the cell */
		whichFace = binarySearch( randomPolygon );
		//printf("whichFace: %d\n ", whichFace );
		if ( whichFace > NumberFaces )
			errorMsg( "Not proper face number in 'createRandomCells'!" );

		s = ran1( &Seed );
		t = ran1( &Seed );

		/*
		 * Chooses a random point in the polygon.
		 * p is the returned random position
		 */
		square2polygon( whichFace, s, t, &p );

		/*
		 * Check if the cell will have a type picked at random
		 * or not
		 */
		if ( pickRandomFlag )
			type = pickType();
		else
			type = D;

		c = createCell(p, whichFace, type, 0);

		/*
		 * The computation of the barycentric coordinates for each
		 * cell is now computed outside the 'createCell' procedure
		 */
		compBarycentricCoord( c );

		if ( parametOKFlag ){
			assignPrim2Cell( c );
		}else{
			//beep();
			//fprintf( stderr, "Cells will not be parameterized!\n");
		}

		/*
		 * I want to check here that the point just created is
		 * actually inside the polygon
		 */
		if ( !pointInPoly2D( whichFace, c->px, c->py ) )
			errorMsg( "Cell just created is outside the polygon! (distpoints.c)" );

		/* update number of cells this face has */
		faces[whichFace].ncells++;
		insertOnListOfCells(c, faces[whichFace].head, faces[whichFace].tail);

		/*
		 * If there is a texture map to control the creation
		 * of the cells, we proceed and change the type of
		 * the cells based on the information on the texture map
		 *
		 * The textID[0] is the ID to control this
		 */
		if ( texturePresent )
		{
			prim1 = faces[whichFace].prim1;
			if ( prim1 != FALSE ){
				textID = Prim[prim1].textID[0];
				//printf("RandonFace: %d Cinlindro: %d, textura: %d\n", whichFace, Prim[prim1], textID);
				if ( textID < totalNumberTextures && textID != -1 ){
					compParamCoordOneCell( c );
					u = c->p1.u;
					v = c->p1.v;
					textValue =  bilinearInterp( u, v, textID );
					if (textValue.redC == 0.0 && c->ctype == C ){
						//removeFromListOfCells(c, faces[whichFace].head);
						//faces[whichFace].nCellsType[c->ctype]--;
						switchCellType( c, D );
					}
					/*if (textValue.redC == 0.0 && c->ctype == D ){
					 switchCellType( c, C );
					 }*/
				}
			}
		}
		/* fprintf(stderr, "px %3.2f py %3.2f pz %3.2f  ",p.x, p.y, p.z); */
	}

	/* check that all cells have faces assigned */
	for (i=0; i < NumberFaces; i++){
		count += faces[i].ncells;
	}
	if ( count != quantity )
		errorMsg("Number of cells in polygons != Number of cells given!");

	/* All just created cells are inside the polygon. I have to
     set this here for the relaxation to work properly */
	for( i = 0; i < NumberFaces; i++){
		faces[i].vorFacesList =
		(faceType *) malloc(sizeof(faceType) * faces[i].ncells );
		if ( faces[i].vorFacesList == NULL )
			errorMsg( "Out of space for list of voronoi polygons in voronoi.c!");
		for ( j = 0; j < faces[i].ncells; j++){
			faces[i].vorFacesList[j].border = IN;
		}
	}

	/* set flag */
	cellsCreated = TRUE;

	/* put the first round of split events in the queue
     only if we are not at the embryonic stage, that is
     we are only growing pattern WITHOUT growth associated
	 */
	if ( !embryoShape ) postSplitEventsOnQueue();

	//#ifdef GRAPHICS
	//  updtNumberOfIndividualCells();
	//#endif
}

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


/* From genericUtil.c
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void saveCellsFile( const char *fileName )
{

	FILE *fp;
	int len;
	char objFileName[128];
	int i, j;
	CELL *c;

	fprintf( stderr, "ExpFileName = %s\n", fileName );
	if( (fp = fopen(fileName, "w")) == NULL )
		errorMsg("Could not open cells file to white!");

	if ( computingPatternFlag ){
		len = strlen( fileName );
		strncpy( objFileName, fileName, len - 3 );
		objFileName[len-3]='\0';
	}
	else{
		len = strlen( sessionFileName );
		strncpy( objFileName, sessionFileName, len - 8 );
		objFileName[len-8]='\0';
	}

	fprintf( fp, "%s\n", objFileName );

	for( i = 0; i < NumberFaces; i++){
		fprintf( fp, "%d %d\n", i, faces[i].ncells );
		c = faces[i].head->next;
		while ( c != faces[i].tail ){
			fprintf( fp, "%d %lg %lg %lg\n", c->ctype,
					c->x, c->y, c->z );
			c = c->next;
		}
	}

	fclose( fp );

	/* inform the user */
	printf("Cells file saved on file %s\n", fileName );
}

/* From genericUtil.c
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
int loadCellsFile( const char *inpFileName )
{
	FILE *fp;
	int len;
	char objFileName[128], fileName[128];
	int i, j;
	CELL *c;
	CELL_TYPE t;
	Point3D p;
	int whichPolygon, ncells, totalNumberOfCells;

	if( (fp = fopen(inpFileName, "r")) == NULL )
		errorMsg("Could not open cells file to read!");

	len = strlen( sessionFileName );
	/*fprintf( stderr, "len %d\n", len );*/
	strncpy( objFileName, sessionFileName, len - 8 );
	objFileName[len-8]='\0';

	fscanf( fp, "%s\n", &fileName );
	if ( strcmp( fileName, objFileName ) ){
		fprintf( stderr, "fileName = %s objFileName = %s\n",
				fileName, objFileName);
		return FALSE;
	}

	totalNumberOfCells = 0;
	for( i = 0; i < NumberFaces; i++){
		fscanf( fp, "%d %d\n", &whichPolygon, &ncells);
		/*fprintf( stderr, "%d %d\n", whichPolygon, i );*/
		if ( i != whichPolygon ){
			fprintf( stderr, "Face read from file = %d current face = %d\n", whichPolygon, i );
			errorMsg("Faces do not match in read cells!");
		}
		faces[i].ncells = ncells;
		totalNumberOfCells += ncells;
		for ( j = 0; j < faces[i].ncells; j++){
			fscanf( fp, "%d %lg %lg %lg\n", &t, &(p.x), &(p.y), &(p.z) );
			/*fprintf( stderr, "%d %lg %lg %lg\n", t, p.x, p.y, p.z );*/
			c = createCell(p, i, t, 0);
			insertOnListOfCells(c, faces[i].head, faces[i].tail);
			/* Compute this cell's barycentric coordinates */
			compBarycentricCoord( c );

		}
	}
	fclose( fp );

	fprintf( stderr, "Loaded Number of Cells:\n");
	fprintf( stderr, "[C] = %d [D] = %d [E]= %d [F] = %d\n",
			nCellsType[C], nCellsType[D],
			nCellsType[E], nCellsType[F] );

	return totalNumberOfCells;
}

