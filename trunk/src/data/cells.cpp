/*
 *  cells.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------
 * 	cells.c
 *---------------------------------------------------
 */

#include "cells.h"

#include "cellsList.h"
#include "Object.h"
#include "Parameters.h"
#include "../simulation/relax.h"
#include "../simulation/simulation.h"
#include "../util/random.h"
#include "../util/heap.h"
#include "../util/genericUtil.h"
#include "../util/distPoints.h"

/*
 *---------------------------------------------------------------
 *	Local global variables
 *---------------------------------------------------------------
 */


/*
 *-----------------------------------------------------------------------
 *-----------------------------------------------------------------------
 */
void assignPosition2Cell( CELL *c, Point3D p )
{
	c->x = p.x;
	c->y = p.y;
	c->z = p.z;
}

/*
 *-----------------------------------------------------------------------
 *-----------------------------------------------------------------------
 */
void assignPrevPosition2Cell( CELL *c, Point3D p )
{
	c->xp = p.x;
	c->yp = p.y;
	c->zp = p.z;
}
/*
 *-----------------------------------------------------------------------
 *
 * This function receives the information to create
 * a new cell. It allocates memory for the cell and
 * returns a pointer to it
 *
 * INPUT: p -> cell's center position
 *	  whichFace -> which face is the cell attached to
 *	  t -> cell type
 *	  g -> cell generation
 *-----------------------------------------------------------------------
 */
CELL* createCell(Point3D pos, int whichFace, CELL_TYPE t, byte g)
{
	CELL *p;
	Point2D q;
	
	/*
	 * Update the counter on how many cells of each type we have
	 */
	nCellsType[t]++;	
	
	/* Update the count for the face */
	faces[whichFace].nCellsType[t]++;
	
	p = cellAlloc();
	p->x = pos.x;
	p->y = pos.y;
	p->z = pos.z;
	p->xp = pos.x;
	p->yp = pos.y;
	p->zp = pos.z;
	p->fx = 0.0;
	p->fy = 0.0;
	p->whichFace = whichFace;
	mapOntoPolySpace( whichFace, p->x, p->y, p->z, &q );
	p->px = q.x;
	p->py = q.y;
	p->prim1 = FALSE;
	p->prim2 = FALSE;
	/* texture coordinates */
	p->p1.u = -1;
	p->p1.v = -1;
	p->p2.u = -1;
	p->p2.v = -1;
	
	p->gen = g;
	
	p->ctype = t;
	p->tsplit = getTimeSplit( Ortcell[p->ctype].rateOfDivision );
	
	return( p );
}
/*
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
float getTimeSplit( float mean )
{
	
	if ( RandomTypeDist == POISSON )
		return( poidev( mean, &Seed ) );
	
	else if ( RandomTypeDist == EXPONENTIAL )
    /*return( gamdev( mean, &Seed ) );*/
		return( mean * gamdev( 1, &Seed ) );
	
	else errorMsg( "Random distribution not specified!" );
	
}

/*
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
CELL_TYPE pickType( void )
{
	float x;
	
	x = ran1(&Seed);
	
	if ( x <= Ortcell[C].prob) return(C);
	else if ( x <= (Ortcell[C].prob + Ortcell[E].prob )) return(E);
	else if ( x <= (Ortcell[C].prob + Ortcell[E].prob + Ortcell[F].prob )) return(F);
	else return(D);
	
}
/*
 *--------------------------------------------------------------------
 *     	Given a cell 'c', this function will split it
 *	in 2, ie, the 'c' plus its offspring
 *
 * 	PS: ??I have to check that when I am including a new cell
 * 	the time for split can not be smaller than the
 * 	present time
 *--------------------------------------------------------------------
 */
void handleSplit( CELL *c )
{
	//printf("dividindo célula... face: %d \n", c->whichFace);
	double nx, ny;
	CELL *child;
	CELL_TYPE childType;
	static int prevNumberCells = 0;
	double rateRadius;
	EVENT motherCellEvent, childCellEvent;
	Point3D p;
	
	/* first value for prev number of cells */
	if ( prevNumberCells == 0 ){
		prevNumberCells = NumberCells;
	}
	
	/* increments the total number of cells */
	NumberCells++;
	
	/*
	 * This part here reduces the size of the radius as
	 * new cells are being added to the simulation. More
	 * cells mean that the average ideal area for a cell
	 * is smaller. I'll be doing this only after a given number of
	 * cells is created and if we are NOT growing the model
	 * and pattern at the same time
	 */
#ifdef a
	erroMsg("Should not get into here!");
	if ( !computingPatternFlag ){
		rateRadius = (float) NumberCells / (float) prevNumberCells;
		
		if ( rateRadius >= 1.2){
			/*
			 * Compute the new radius of repulsion. FALSE
			 * is just a flag to say it is not the first
			 * time we are doing it
			 */
			compRadiusRepulsion( FALSE );
			/*fprintf( stderr, "\n\nprev = %d current = %d\n\n",
			 prevNumberCells, numberCells );*/
			prevNumberCells = NumberCells;
		}
	}
#endif
	
	/* increments the parent cell's generation. I AM NOT USING THIS YET!*/
	c->gen++;
	
	/* set a random position for new child cell */
	pickRandomPosition( c, &p );
	
	/* Check here if the new cell should be of a different
     type than its mother */
	childType = getNewType(c->ctype);
	
	/* creates child cell */
	child = createCell(p, c->whichFace, childType, 0);
	/* set the primitives of the child cell equal to its mother */
	child->prim1 = c->prim1;
	child->prim2 = c->prim2;
	/* insert the child cell on the face list */
	insertOnListOfCells( child, faces[c->whichFace].head, faces[c->whichFace].tail );
	/*
	 * Update the counter for this face. If the child cell is NOT
	 * on this same face this is going to be fixed later by the
	 * 'keepCellOnSurface' procedure
	 */
	faces[c->whichFace].ncells++;
	
	/*
	 * In order for the procedure 'keepCellOnSurface' to work,
	 * I need to set the previous position of the cell as
	 * being equal to the current position of its mother
	 */
	child->xp = c->x;
	child->yp = c->y;
	child->zp = c->z;
	
	/*
	 * If the random position of the child cell is outside
	 * the current face, the 'keepCellOnSurface' will move
	 * the cell until it rests on a proper face
	 */
	keepCellOnSurface( child );
	
	/*
	 * Updates next split time for both cells
	 * I have to reschedule the time for the splitting
	 * of the new cell, since the 'createCell' routine
	 * is not 'sensitive' to the situation where I am
	 * computing pattern and growth together
	 */
	if ( computingPatternFlag ){
		if ( c->whichFace == child->whichFace ) 
			compSplittingRatesForOneFace( c->whichFace );
		else{
			compSplittingRatesForOneFace( c->whichFace );
			compSplittingRatesForOneFace( child->whichFace );
		}
		child->tsplit = c->tsplit + getTimeSplit( faces[child->whichFace].rates[child->ctype] );
		c->tsplit += getTimeSplit( faces[c->whichFace].rates[c->ctype] );
		if ( c->tsplit < 0 || child->tsplit < 0 ){
			fprintf( stderr, "Face mother = %d C: number = %d rate= %lg D: number = %d rate = %lg\n",
					c->whichFace,
					faces[c->whichFace].nCellsType[C], faces[c->whichFace].rates[C],
					faces[c->whichFace].nCellsType[D], faces[c->whichFace].rates[D] );
			fprintf( stderr, "Face child = %d C: number = %d rate= %lg D: number = %d rate = %lg\n",
					child->whichFace,
					faces[c->whichFace].nCellsType[C], faces[child->whichFace].rates[C],
					faces[c->whichFace].nCellsType[D], faces[child->whichFace].rates[D] );
			errorMsg("Split time negative! (cells.c)");
		}
	}
	else{
		child->tsplit += c->tsplit;
		c->tsplit += getTimeSplit( Ortcell[c->ctype].rateOfDivision );
	}
	/*
	 * Now that I am sure that the new cell's position is
	 * inside a polygon, I can compute its barycentric
	 * coordinates
	 */
	compBarycentricCoord( child );
	
	/* includes mother's next split event into the heap */
	motherCellEvent.whichEvent = SPLIT;
	motherCellEvent.eventTime =  c->tsplit;
	motherCellEvent.info = c;
	HeapInsert( myheap, &motherCellEvent );
	/* includes child's next split event into the heap */
	childCellEvent.whichEvent = SPLIT;
	childCellEvent.eventTime =  child->tsplit;
	childCellEvent.info = child;
	HeapInsert( myheap, &childCellEvent );
	
	//#ifdef GRAPHICS
	//  fl_set_counter_value(myformCM->numberOfCells, (double) NumberCells);
	//  updtNumberOfIndividualCells();
	//#endif
}

/*
 *------------------------------------------------------
 *------------------------------------------------------
 */
void pickRandomPosition( CELL *c, Point3D *p )
{
	double x, y;
	Point2D q;
	
	/* HOW DO I GUARANTEE THAT THESE c->px and c->py ARE ALREADY IN THE NEW
     POLYGON'S SIZE COORDINATES? */
	/* get cell's current position in the 2D space */
	mapOntoPolySpace(c->whichFace, c->x, c->y, c->z, &q);
	
	x = q.x + fran(-radiusRep*0.01, radiusRep*0.01);
	y = q.y + fran(-radiusRep*0.01, radiusRep*0.01);
	
	//if ( (q.x - c->px) != 0 || (q.y - c->py) != 0)
	//	fprintf( stderr, "dif: %lg %lg\n", q.x - c->px, q.y - c->py );
	
	/*x = c->px + fran(-radiusRep*0.01, radiusRep*0.01);
	 y = c->py + fran(-radiusRep*0.01, radiusRep*0.01);*/
	
	mapFromPolySpace(c->whichFace, x, y, p);
	/*fprintf( stderr, "Offspring: mother's position %3.2f %3.2f %3.2f\n",
	 c->x, c->y, c->z );
	 fprintf( stderr, "Child: %3.2f %3.2f %3.2f\n", p->x, p->y, p->z );*/
	
	return;
    
}
/*
 *-----------------------------------------------------------
 * Switch a Cell, ie, makes the cell 'c' received as a type
 * 'new_type'
 *-----------------------------------------------------------
 */
void switchCellType(CELL *c, CELL_TYPE newType)
{
	int i;
	CELL *s;
	
	nCellsType[c->ctype]--;
	nCellsType[newType]++;
	c->ctype = newType;
	c->tsplit = getTimeSplit( Ortcell[c->ctype].rateOfDivision );
	c->gen = 0;
    
	//#ifdef GRAPHICS
	//  updtNumberOfIndividualCells();
	//#endif
}

/*
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
CELL_TYPE getNewType( int type )
{
	float x;
	
	x = ran1(&Seed);
	
	if ( x <= swTypes[type][C]) return(C);
	else if ( x <= (swTypes[type][C] + swTypes[type][D])) return(D);
	else if ( x <= (swTypes[type][C] + swTypes[type][D] + swTypes[type][E])) return(E);
	else return(F);
}

/*
 *--------------------------------------------------------------
 * Returns a float in the range (min,max) with
 * uniform distribution
 *--------------------------------------------------------------
 */
double fran(double min, double max)
{
	return(min + ran1(&Seed)*(max-min));
}

/*
 *------------------------------------------------------------------------
 *  This function initializes the information about the cell's types
 *------------------------------------------------------------------------
 */
void initCellsInfo(void)
{
	double sumOfRatesOfSplitting;
	
	/* init info for cell type C */
	Ortcell[C].color[R] = COR_TYPE_C_R;
	Ortcell[C].color[G] = COR_TYPE_C_G;
	Ortcell[C].color[B] = COR_TYPE_C_B;
	Ortcell[C].prob = INIT_PROB_C;
	Ortcell[C].rateOfDivision = INIT_RATE_DIV_C;
	
	/* init info for cell type D */
	Ortcell[D].color[R] = COR_TYPE_D_R;
	Ortcell[D].color[G] = COR_TYPE_D_G;
	Ortcell[D].color[B] = COR_TYPE_D_B;
	Ortcell[D].prob = INIT_PROB_D;
	Ortcell[D].rateOfDivision = INIT_RATE_DIV_D;
	
	/* init info for cell type E */
	Ortcell[E].color[R] = COR_TYPE_E_R;
	Ortcell[E].color[G] = COR_TYPE_E_G;
	Ortcell[E].color[B] = COR_TYPE_E_B;
	Ortcell[E].prob = INIT_PROB_E;
	Ortcell[E].rateOfDivision = INIT_RATE_DIV_E;
	
	/* init info for cell type F */
	Ortcell[F].color[R] = COR_TYPE_F_R;
	Ortcell[F].color[G] = COR_TYPE_F_G;
	Ortcell[F].color[B] = COR_TYPE_F_B;
	Ortcell[F].prob = INIT_PROB_F;
	Ortcell[F].rateOfDivision = INIT_RATE_DIV_F;
	
	sumOfRatesOfSplitting =
	Ortcell[C].rateOfDivision +
	Ortcell[D].rateOfDivision +
	Ortcell[E].rateOfDivision +
	Ortcell[F].rateOfDivision;
	
	/*
	 * Compute the relative rates
	 * Marcelo, I am not sure how to get the relative rates
	 * working yet, so in the meantime, I am getting for
	 * the C cell the value for D and vice-versa
	 */
	/*Ortcell[C].relRate = Ortcell[D].rateOfDivision / sumOfRatesOfSplitting;
	 Ortcell[D].relRate = Ortcell[C].rateOfDivision / sumOfRatesOfSplitting;*/
	
	Ortcell[C].relRate = 1.0 - (Ortcell[C].rateOfDivision / sumOfRatesOfSplitting);
	Ortcell[D].relRate = 1.0 - (Ortcell[D].rateOfDivision / sumOfRatesOfSplitting);
	
	Ortcell[E].relRate = Ortcell[E].rateOfDivision / sumOfRatesOfSplitting;
	Ortcell[F].relRate = Ortcell[F].rateOfDivision / sumOfRatesOfSplitting;
	
}


