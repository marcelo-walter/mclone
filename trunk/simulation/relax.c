/*
 *---------------------------------------------------------
 *	relax.c
 *	All functions related with the relaxation
 *	procedure
 *---------------------------------------------------------
 */


#include <math.h>

#include <stdio.h>
#include "../common.h"
#include "relax.h"
#include "forces.h"
/* Added by Thompson Peter Lied in 16/07/2002 */
//#include "../data/Macros.h"

//#ifdef GRAPHICS
//#include "sgi/myforms.h"
//#endif

/*
 *------------------------------------------------------------
 *	External function definitions
 *------------------------------------------------------------
 */
extern int  findEdgeForVert( int vstart, int vend, int *direction );
extern int  linesIntersect( Point2D p1, Point2D p2, Point2D p3,
						   Point2D p4, Point2D *i, double *t );
extern int  goodLinesIntersect( Point2D p1, Point2D p2, Point2D p3,
							   Point2D p4, Point2D *i, double *t );
extern void assignPosition2Cell( CELL *c, Point3D p );
extern void assignPrevPosition2Cell( CELL *c, Point3D p );

/* added bu Thompson Peter Lied in 15/07/2002 */
char outputCMfileName[128];
/* end */

/*
 *--------------------------------------------------
 *	Externally Defined Global variables
 *--------------------------------------------------
 */
//#ifdef GRAPHICS
//extern FD_control *myform;
//extern FD_cm *myformCM;
//#endif

/*
 * The initingFlag flags to the relaxation
 * routine that we DO NOT want the adhesion
 * between cells to play any role at this
 * stage. It is only used in this context.
 */
extern flag initingFlag;

/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */
double 	radiusRep = 0.0;
double  radiusRepSquare = 0.0;
double 	wrp;		/* weight for the repulsive radius */
int 	iterPerUnitTime;

/* global weight for relaxation forces: steady and initial */
/* double 	wForce, initWForce; */
double wd, initWForce;           /* Added by Thompson Peter Lied in 16/07/2002 */

/* weights for anisotropic effects */
/* double 	wx, wy; */
double wa;           /* Added by Thompson Peter Lied in 16/07/2002 */

/* area of one cell */
double  AreaOneCell;

/* initial number of steps for the relaxation */
int 	initNumRelax;

/* Debug information. How many times cells changed
 faces */
int nChangeFaces = 0;

/* Added by Thompson Peter Lied in 15/07/2002 */
/* orientation vector */
double Ox, Oy, OxSq, OySq;

/* orientation in degrees for the anisotropy */
float orientation;

/* Anisotropy effects */
double AniX, AniY, AniCommon;

double Ani;


/* end */


/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
/* Modified by Fabiane Queiroz in 25/11/2009 
 The initial relaxation process is always a global process*/

void initialRelaxation()
{
	
	Array listFaces;
	listFaces.array = NULL;
	listFaces.size = 0;
	double actualWForce;
	
	/*
	 * The role of initingFlag is to relax the initial
	 * set of cells without adhesion playing any role.
	 * It's used inside the force computation routine
	 * defined in forces.c
	 */
	initingFlag = TRUE;
	/*
     actualWForce = wForce;
     wForce = initWForce;
	 */
	actualWForce = wd;    /* Added by Thompson Peter Lied in 16/07/2002 */
	wd = initWForce;
	/* do initial relaxation steps */
	printf("Relaxing init set of cells %d steps without differential adhesion! (wd initial = %f )\n",
		   initNumRelax, initWForce );
	
	relaxation( initNumRelax, GLOBALRELAX, &listFaces );
	
	initingFlag = FALSE;
	/* wForce = actualWForce; */
	wd = actualWForce;    /* Added by Thompson Peter Lied in 16/07/2002 */
	
	// printf("Done!\n");
	
	
}
/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void initRelax( void )
{
	wrp = WRP;
	/* added by Thompson Peter Lied in 15/07/2002 */
	/*
	 wForce = WFORCE;
	 wx = WX;
	 wy = WY;
	 */
	wd = WD;
	wa = WA;
	orientation = ORIENTATION;
	initWForce = INITWFORCE;
	initNumRelax = INITNUMRELAX;
	NumberCells = NCELLS;
	iterPerUnitTime = ITER_PER_UNIT_TIME;
}
/*
 *--------------------------------------------------
 * Compute the radius of repulsion based on
 * the area of the object and the number of
 * cells
 *--------------------------------------------------
 */
void compRadiusRepulsion( int firstTime )
{
	
	AreaOneCell = TotalAreaOfObject / NumberCells;
	radiusRep = sqrt ( AreaOneCell );
	
	/* multiply weigthing factor */
	radiusRep *= wrp;
	
	/* radius squared is used when computing the forces */
	radiusRepSquare = radiusRep * radiusRep;
	
	//printf("Total Area = %3.2f Radius (for %d cells) = %3.2f\n",
	// TotalAreaOfObject, NumberCells, radiusRep );
	//printf("Avg area of triangles = %lg Area of One Cell = %lg\n",
	// TotalAreaOfObject/ NumberFaces, AreaOneCell );
}



/*
 *-----------------------------------------------------------------------
 *      Added by Thompson Peter Lied in 16/07/2002
 *-----------------------------------------------------------------------
 */
void compOrientation( void )
{
	double rad;
	
	//orientation =0.0;
	
	rad = orientation * PI / 180.0;
	Ox = cos(rad);
	Oy = sin(rad);
	OxSq = Ox * Ox;
	OySq = Oy * Oy;
	fprintf(stderr,"\nOrientation = %f \n", orientation);
	fprintf(stderr,"rad = %f  Ox = %f  Oy = %f \n", rad, Ox, Oy);
}


/* Added by Thompson Peter Lied in 16/07/2002 */
/*
 *-----------------------------------------------------------------
 *  Anisotropy effects. According Marcelow Thesis
 *----------------------------------------------------------------
 */
void comp_aniso_effect( void )
{
	// AniX = wa * wd * Ox;
	// AniY = wa * wd * Oy;
	//AniX = wa * Ox;
	//AniY = wa * Oy;
	Ani = wa; 
	AniCommon = (1 - wa) * wd;
	fprintf(stderr,"wa = %f, wd = %f \n", wa, wd);
	// fprintf(stderr,"AniX = %f  AniY = %f  AniCommon = %f \n", AniX, AniY, AniCommon );
	fprintf(stderr,"Ani = %f  AniCommon = %f \n", Ani, AniCommon );
}


/*
 *-----------------------------------------------------------------------
 * This function goes over all cells in the surface and computes a
 * new position for them according to a 
 procedure
 *-----------------------------------------------------------------------
 */

/*Modified by Fabiane Queiroz in 25/11/2009 : add the Local Relaxation Process*/

void relaxation( int steps, RELAXMODE rMode, Array *listFaces )
{	
	int n;
	if(rMode == GLOBALRELAX)
	{	
		//printf("GlobalRelax, com %d passos de relaxamento\n", steps);
		//#ifdef GRAPHICS
		//  fprintf( stderr, ". ");
		//  winDisplay();
		//#endif
		
		for(n = 0; n < steps; n++){
			fprintf( stderr, "%d ", n );
			
			/*
			 * Compute all forces for each cell
			 */
			compForces(listFaces);
			//printf("comForces\n");
			/*
			 * Update the new positions for the cells
			 */
			updatePositions(listFaces);
			//printf("updatePositions\n");
			//#ifdef GRAPHICS
			//    winDisplay();
			//    fl_set_counter_value(myformCM->numberOfCells, (double) NumberCells);
			//#endif
			
		}
		/* end for number of relaxation steps */
		/*savePrimitivesFile("teste.prim", NumberPrimitives);
		 saveObjFile( "teste.obj" );
		 saveCellsFile("teste.cm");*/
		/*
		 * Once we have all cells in their new positions
		 * I compute the cells' coordinates in polygon
		 * space, since those are the coordinates used
		 * by the voronoi procedure. I also update the new
		 * number of cells per face
		 */
		compCellsPolygonCoordinates(listFaces);
		//printf("AAAAAAA!\n");   
		/*
		 * Re-assign primitive information for all cells
		 */
		//savePrimitivesFile("teste1.prim", NumberPrimitives);
		//saveObjFile( "teste1.obj" );
		
		/*savePrimitivesFile("teste.prim", NumberPrimitives);
		 saveObjFile( "teste.obj" );
		 saveCellsFile("teste.cm");*/
		assignPrim2AllCells(listFaces);
		//printf("EEEEEE! \n");
		/*
		 * If the model was parameterized, we can also
		 * re-compute the cells' cylindrical coordinates
		 */
		compParamCoordAllCells(listFaces);
		//printf("OOOOO! \n");
	}
	else if(rMode == LOCALRELAX)
	{
		
		if(listFaces->array != NULL)
		{
			/*int j;
			 for (j=0; j <listFaces->size; j++)
			 printf("Elemento = %d , Valor = %d\n", j, listFaces->array[j]);*/
			
			for(n = 0; n < steps; n++)
			{
				fprintf( stderr, "%d ", n );
				
				/*
				 * Compute all forces for each cell of the listFaces
				 */
				compForces(listFaces);
				
				/*
				 * Update the new positions for the cells into the listFaces
				 */
				updatePositions(listFaces);
				
			} /* end for number of relaxation steps */
			
			/*
			 * Once we have all cells in their new positions
			 * I compute the cells' coordinates in polygon
			 * space, since those are the coordinates used
			 * by the voronoi procedure. I also update the new
			 * number of cells per face
			 */
			compCellsPolygonCoordinates(listFaces);
			
			/*
			 * Re-assign primitive information for all cells
			 */
			assignPrim2AllCells(listFaces);
			
			/*
			 * If the model was parameterized, we can also
			 * re-compute the cells' cylindrical coordinates
			 */
			compParamCoordAllCells(listFaces);
			
		}else
			printf("Sem lista de Faces!\n");	
	}
	else
	{
		printf("Relaxed option not mentioned in the parameter file.");
		return;
	}
	// printf("Done! \n");
	
}
/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
/*Modified by fabiane Queiroz in 01/12/2009*/
void updatePositions(Array* listFaces )
{
	int i;
	double fMin, fMax, minForce, maxForce, avgForce, avgF;
	
	maxForce = 1e-10;
	minForce = 1e+10;
	avgForce = 0.0;
	avgF = 0.0;
	
	//int nFaces;
	
	/*if(listFaces->array == NULL)
	 nFaces = NumberFaces;
	 else
	 nFaces = listFaces->size;
	 printf("Em updatePositins: nFaces = %d \n", nFaces);*/
	
	/*Global relaxation*/
	if(listFaces->array == NULL)
	{
		for( i = 0; i < NumberFaces; i++ )
		{
			updatePositionsForFace( i, &fMin, &fMax, &avgF );
			if ( fMin < minForce ) minForce = fMin;
			if ( fMax > maxForce ) maxForce = fMax;
			avgForce += avgF;
		}
		avgForce /= (double) NumberFaces;
	}
	/*local relaxation*/
	else
	{
		for( i = 0; i < listFaces->size; i++ )
		{
			updatePositionsForFace( listFaces->array[i], &fMin, &fMax, &avgF );
			if ( fMin < minForce ) minForce = fMin;
			if ( fMax > maxForce ) maxForce = fMax;
			avgForce += avgF;
		}
		avgForce /= (double) listFaces->size;
	}
	/*printf( "%lg %lg %lg %lg\n", radiusRep, minForce, maxForce, avgForce );*/
}
/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void updatePositionsForFace( int whichFace, double *fMin, double *fMax, double *avgF )
{
	CELL *c, *nextCell;
	Point2D q;
	Point3D p;
	double f, maxForce, minForce, avgForce;
	int cellsCount;
	double tmpx, tmpy;
	
	maxForce = 1e-10;
	minForce = 1e+10;
	avgForce = 0;
	cellsCount = 0;
	
	int contador = 0;
	
	c = faces[whichFace].head->next;
	while( c != faces[whichFace].tail )
	{
		nextCell = c->next;
		if ( c->fx != 0 && c->fy != 0 )
		{
			cellsCount++;
			/* compute new position */
			/* wx and wy include anisotropy effect */
			/* Removed by Thompson Peter Lied in 16/07/2002 */
			/* c->fx *= wForce * wx;
			 c->fy *= wForce * wy;
			 */
			
			/* compute new position */   
			/* Modified by Fabiane Queiroz in 24/09/2009 */
			/* c->fx *= AniCommon * AniX;
			 c->fy *= AniCommon * AniY;
			 */
			
			c->fx *= AniCommon + AniX;
			c->fy *= AniCommon + AniY;
			
			//printf("c->fx= %f e c->fy= %d\n", c->fx, c->fy);
			
			/*//printf("c->fx= %f \n", c->fx);
			 /* Added by Thompson Peter Lied in 15/07/20002 */
			// tmpx = c->fx;
			//tmpy = c->fy;
			
			//printf("dix = %f, diy = %f \n", tmpx, tmpy);
			
			/* compute new position
			 include anisotropy effect */
			//c->fx = AniX + (AniCommon * tmpx);
			// c->fy = AniY + (AniCommon * tmpy);
			
			
			
			f = sqrt( c->fx * c->fx + c->fy * c->fy );
			avgForce += f;
			
			if ( f < minForce ) minForce = f;
			if ( f > maxForce ) maxForce = f;
			
			/* get cell's current position in the 2D space */
			mapOntoPolySpace(c->whichFace, c->x, c->y, c->z, &q);
			
			/* update the new position on the polygon's 2D space */
			q.x += c->fx;
			q.y += c->fy;
			
			/* p will return the point in the original 3D space */
			mapFromPolySpace(c->whichFace, q.x, q.y, &p);
			/*
			 * Finally update the new position
			 * I want to make sure that the new position
			 * passes the inside polygon test
			 */
			if ( cellInPoly( c, TRUE ) == FALSE )
			{
				contador = contador +1;
				//fprintf( stderr, "Face %d\n", c->whichFace );
				//fprintf( stderr, "cell's current position (%lg %lg %lg)\n",
					//	c->x, c->y, c->z );
				//fprintf( stderr, "cell's previous position (%lg %lg %lg)\n",
					//	c->xp, c->yp, c->zp );
				/* Added by Thompson Peter Lied in 16/07/2002 */
				//fprintf( stderr, "###ExpFileName = %s\n", outputCMfileName);  
				saveCellsFile( outputCMfileName );
				/* end */
				//errorMsg("Original cell position outside polygon!");
			}
			if ( cellInPoly( c, FALSE ) == FALSE )
			{
				contador = contador +1;
				//fprintf( stderr, "Face %d\n", c->whichFace );
				//fprintf( stderr, "cell's current position (%lg %lg %lg)\n",
				//		c->x, c->y, c->z );
				//fprintf( stderr, "cell's previous position (%lg %lg %lg)\n",
					//	c->xp, c->yp, c->zp );
				/* Added by Thompson Peter Lied in 16/07/2002 */
				saveCellsFile( outputCMfileName );
				/* end */
				//errorMsg("Previous cell position outside polygon!");
			}
			/* Assign the cell's previous position */
			c->xp = c->x;
			c->yp = c->y;
			c->zp = c->z;
			assignPosition2Cell( c, p );
			keepCellOnPlane( c );
			keepCellOnSurface( c );
		}
		c = nextCell;
	}
	if ( cellsCount != 0 )
		*avgF = avgForce / (double) cellsCount;
	else *avgF = avgForce;
	
	*fMin = minForce;
	*fMax = maxForce;
	if(contador != 0)
		printf("contador: %d\n",contador);
	
}
/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 01/12/2009*/
void compForces(Array* listFaces)
{
 	int i;
	
	/*int j;
	 for (j=0; j <listFaces->size; j++)
	 printf("Elemento = %d , Valor = %d\n", j, listFaces->array[j]);*/
	
	/*int nFaces;	
	 if(listFaces->array == NULL){
	 //printf("CompForces: array = NULL\n");
	 nFaces = NumberFaces;}
	 else{
	 nFaces = listFaces->size;}
	 //printf("CompForces: array = %d \n", nFaces);}
	 
	 for( i = 0; i < nFaces; i++ ){	
	 //printf("Face = %d \n", i);
	 compForcesForFace( i );}*/
	
	/*if RMode is global*/
	if(listFaces->array == NULL)
	{
		for(i = 0; i< NumberFaces; i++)
			compForcesForFace(i);
	}
	/*if RMode is local*/
	else
	{
		int j;
		for(i = 0; i<listFaces->size; i++){
			j = listFaces->array[i];
			compForcesForFace(j);
		}
		
	}
}

/*
 *--------------------------------------------------
 *	For each cell, compute its coordinates
 *	in polygon space. Also, update the
 *	count of cells in each face, since
 *	many cells have changed faces
 *--------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 01/12/2009*/
void compCellsPolygonCoordinates(Array* listFaces)
{
	int i;
	int totalNumberOfCells, cellsOnThisFace;
	int numberPerType[HOW_MANY_TYPES];
	CELL *c;
	Point2D q;
	
	totalNumberOfCells = 0;
	
	/*int nFaces;
	 
	 if(listFaces->array == NULL)
	 nFaces = NumberFaces;
	 else
	 nFaces = listFaces->size;*/
	
	//printf("Em  compCellsPolygonCoordinates: nFaces = %d \n", nFaces);
	
	if(listFaces->array == NULL)
	{
		for( i = 0; i < NumberFaces; i++ )
		{
			cellsOnThisFace = 0;
			numberPerType[C] = 0; numberPerType[D] = 0;
			numberPerType[E] = 0; numberPerType[F] = 0;
			c = faces[i].head->next;
			while( c != faces[i].tail )
			{
				mapOntoPolySpace(c->whichFace, c->x, c->y, c->z, &q);
				c->px = q.x;
				c->py = q.y;
				cellsOnThisFace++;
				numberPerType[c->ctype]++;
				c = c->next;
			}
			/* Transfer the information to the faces data-structure */
			faces[i].ncells = cellsOnThisFace;
			faces[i].nCellsType[C] = numberPerType[C];
			faces[i].nCellsType[D] = numberPerType[D];
			faces[i].nCellsType[E] = numberPerType[E];
			faces[i].nCellsType[F] = numberPerType[F];
			totalNumberOfCells += cellsOnThisFace;
		}
		
	}
	else
	{
		int j;
		for( i = 0; i < listFaces->size; i++ )
		{
			j = listFaces->array[i];
			
			cellsOnThisFace = 0;
			numberPerType[C] = 0; numberPerType[D] = 0;
			numberPerType[E] = 0; numberPerType[F] = 0;
			c = faces[j].head->next;
			while( c != faces[j].tail )
			{
				mapOntoPolySpace(c->whichFace, c->x, c->y, c->z, &q);
				c->px = q.x;
				c->py = q.y;
				cellsOnThisFace++;
				numberPerType[c->ctype]++;
				c = c->next;
			}
			/* Transfer the information to the faces data-structure */
			faces[j].ncells = cellsOnThisFace;
			faces[j].nCellsType[C] = numberPerType[C];
			faces[j].nCellsType[D] = numberPerType[D];
			faces[j].nCellsType[E] = numberPerType[E];
			faces[j].nCellsType[F] = numberPerType[F];
			totalNumberOfCells += cellsOnThisFace;
		}
	}
	
	
	/* I want to make sure that the total number of
	 cells is the same as the sum of cells per
	 face */
	if ( totalNumberOfCells != NumberCells)
	{
		//fprintf( stderr, "Cells from faces = %d -- Cells = %d\n",totalNumberOfCells, NumberCells);
		/*errorMsg("Number of cells discrepancy!");*/
	}
	
	
	
}


/*
 *-------------------------------------------------------
 *	Project cell onto plane of cell's current face
 *------------------------------------------------------
 */
void
keepCellOnPlane( CELL *c )
{
	double t;
	
	/* project point onto plane of point's current face */
	t = -(faces[c->whichFace].planeCoef.a * c->x +
		  faces[c->whichFace].planeCoef.b * c->y +
		  faces[c->whichFace].planeCoef.c * c->z +
		  faces[c->whichFace].planeCoef.d);
	
	c->x += t * faces[c->whichFace].planeCoef.a;
	c->y += t * faces[c->whichFace].planeCoef.b;
	c->z += t * faces[c->whichFace].planeCoef.c;
}

/*
 *--------------------------------------------------
 *	Project point onto plane of face 'wichFace'
 *--------------------------------------------------
 */
void
mapPointOntoPlane( Point3D *p, int whichFace )
{
	double t;
	
	t = -(faces[whichFace].planeCoef.a * p->x +
		  faces[whichFace].planeCoef.b * p->y +
		  faces[whichFace].planeCoef.c * p->z +
		  faces[whichFace].planeCoef.d);
	
	p->x += t * faces[whichFace].planeCoef.a;
	p->y += t * faces[whichFace].planeCoef.b;
	p->z += t * faces[whichFace].planeCoef.c;
	
}
/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void keepCellOnSurface( CELL *c )
{
//printf("KeepecellOnSuerface!\n");	
#define N_OF_TRIES 30
	
	int whichEdge, newFace, direction;
	int nOfTries = 0, originalFace;
	Point3D p, intersec, originalPos;
	double d = 0, t;
	
	/*
	 * At this point, the previous cell position (xp,yp,zp)
	 * contains the cell position BEFORE the force has
	 * been applied. In case we cannot find a new position
	 * for this cell, it will return to its original
	 * position. That's why I am keeping the record of
	 * the cell's original position here
	 */
	originalFace  = c->whichFace;
	originalPos.x = c->xp;
	originalPos.y = c->yp;
	originalPos.z = c->zp;
	
	
#ifdef VERBOSE
	fprintf( stderr, "\nStarting point!\n");
	fprintf( stderr, "Previous position %f %f %f in face %d\n",
			c->xp, c->yp, c->zp, c->whichFace );
	fprintf( stderr, "Current position %f %f %f\n",
			c->x, c->y, c->z );
#endif
	
	/*
	 * The routine 'cellInPolyAndEdge' will return either -1, if the cell
	 * new position is INSIDE the polygon, OR the edge index for the
	 * edge crossed. We keep doing this until either the cell finds
	 * a position to rest, or a given number of tries has been reached
	 */
	//printf("primeiro if!!\n");
	if ( (whichEdge = cellInPolyAndEdge( c, &direction, &intersec, &t, -1 )) == -1 )
	{
		//printf("ufa passei!!!\n");	
		return;
	}
	else if((whichEdge = cellInPolyAndEdge( c, &direction, &intersec, &t, -1 )) == -2){
		killCell(c);
	}
	else
	{
		//fprintf( stderr, "uffa passei mas fui pro else!!!!\n");
		while ( whichEdge != -1 && nOfTries < N_OF_TRIES )
		{
			
			nOfTries++;
			
			/*
			 * Computes the distance between cell's current and
			 * previous position. I am not yet using this distance
			 * as a metric to check if the cell is going to rest
			 */
			d = sqrt((c->x - c->xp) * (c->x - c->xp) +
					 (c->y - c->yp) * (c->y - c->yp) +
					 (c->z - c->zp) * (c->z - c->zp));
			//fprintf( stderr, "d = %f \n", d );
			
#ifdef VERBOSE
			fprintf( stderr, "Intersection w/ edge %d pface %d nface %d\n",
					whichEdge, edges[whichEdge].pf, edges[whichEdge].nf );
			fprintf( stderr, "%f %f %f at t = %f dir = %d\n",
					intersec.x, intersec.y, intersec.z, t, direction );
			fprintf( stderr, "distance %f\n", d );
#endif
			
			/* if point no longer in polygon then we need to move it
			 to an adjacent one */
			p.x = c->x;
			p.y = c->y;
			p.z = c->z;
			/*
			 * Direction TRUE means that we found an edge in the
			 * 'right' direction, ie, the cell is going from
			 * a 'p' face to a 'n' face
			 */
			if ( direction )
			{
				V3PreMul( &p, &(edges[whichEdge].pn) );
				V3PreMul( &intersec, &(edges[whichEdge].pn) );
				newFace = edges[whichEdge].nf;
			//	printf("Entrei no direction!\n");
			}
			else
			{
				V3PreMul( &p, &(edges[whichEdge].np) );
				V3PreMul( &intersec, &(edges[whichEdge].np) );
				newFace = edges[whichEdge].pf;
				//printf("Entrei no else do direction!\n");
			}
			
			/* this should be the point's position in the new adjacent face */
			assignPosition2Cell( c, p );
			
			/* the new previous position is the intersection just computed */
			assignPrevPosition2Cell( c, intersec );
			
			/* change the face information attached to the cell */
			changeFacesList(c, newFace, c->whichFace);
			c->whichFace = newFace;
			nChangeFaces++;
			
#ifdef VERBOSE
			fprintf( stderr, "Went to %f %f %f and face %d\n",
					c->x, c->y, c->z, c->whichFace);
#endif
			
			if ( cellInPoly( c, FALSE ) == FALSE )
			{
				fprintf( stderr, "Face %d\n", c->whichFace );
				errorMsg("Previous cell position outside the face! (keepCellOnSurface)");
			}
			//printf("Vou computar!!!\n");
			whichEdge = cellInPolyAndEdge( c, &direction, &intersec, &t, whichEdge );
			//printf("Computei!\n");
		}
		/*
		 * At this point I am assuming that the cell has found a new
		 * polygon to live and this polygon is different from
		 * the original polygon. It is very likely that the primitive
		 * information also changed and I need therefore to recompute it
		 * here
		 */
		if ( whichEdge == -1 )
		{
			//fprintf( stderr, "I am recomputing primitive information for a cell!\n" );
			assignPrim2Cell( c );
		}
		
	}
	/*
	 * If nOfTries >= N_OF_TRIES it means that the cell travelled more than
	 * N_OF_TRIES polygons and still could not find a polygon to rest.
	 * N_OF_TRIES is an arbitrary number which seems to be working now
	 * I might need to review this later. The idea here is to
	 * make the cell go back to its original position in case it cannot
	 * find a polygon to rest
	 */
	if ( nOfTries >= N_OF_TRIES ){
		/* Assign the old position of this cell as its current
		 and previous position */
		assignPosition2Cell( c, originalPos );
		assignPrevPosition2Cell( c, originalPos );
		/* update which face this cell belongs to */
		changeFacesList(c, originalFace, newFace );
		c->whichFace = originalFace;
		fprintf( stderr, "Could not map cell! (distance = %lg)\n", d );
		return;
	}
	
}


/*
 *---------------------------------------------------------
 *	This routine will return the edge index
 *	through which the cell was pushed across.
 *	The variable 'direction' will be TRUE if
 *	the edge was found from vstart->vend and FALSE if
 *	the edge is found from vend->vstart
 *---------------------------------------------------------
 */
int cellInPolyAndEdge( CELL *c, int *direction, Point3D *intersec3D, double *t, int prevEdge )
{
	
	int nverts, whichFace;
	int i, whichEdge, we[3], dir[3], it[3];
	Point2D p, v, qCurrent, qPrevious, intersec[3];
	Point2D v0, v1;
	Point3D p0, p1;
	double z[3], tt[3];
	
	whichFace = c->whichFace;
	nverts = faces[whichFace].nverts;
	/*
	 * Implementation dependent code! Assumes that we are
	 * always working with triangles!
	 */
	if ( nverts != 3 )
		errorMsg("Don't know how to deal with non-triangular faces (relax.c)!");
	/* 
	 * Map the current and previous cell's position to the
	 * 2D space of the face. I want to compute
	 * the intersection between a line defined by the
	 * previous and current cell's position and the edge
	 */
	mapOntoPolySpace(whichFace, c->x, c->y, c->z, &qCurrent );
	mapOntoPolySpace(whichFace, c->xp, c->yp, c->zp, &qPrevious );
	
#ifdef VERBOSE
	fprintf( stderr, "qPrevious [X] = %f [Y] = %f\n", qPrevious.x, qPrevious.y );
	fprintf( stderr, "qCurrent [X] = %f [Y] = %f\n", qCurrent.x, qCurrent.y );
#endif
	
	for(i = 0; i < nverts; i++)
	{
		we[i] = -1;
		tt[i] = -1;
		it[i] = -1;
		dir[i] = -1;
		p0.x = vert[faces[whichFace].v[i]].pos.x;
		p0.y = vert[faces[whichFace].v[i]].pos.y;
		p0.z = vert[faces[whichFace].v[i]].pos.z;
		p1.x = vert[faces[whichFace].v[(i+1)%nverts]].pos.x;
		p1.y = vert[faces[whichFace].v[(i+1)%nverts]].pos.y;
		p1.z = vert[faces[whichFace].v[(i+1)%nverts]].pos.z;
		
		mapOntoPolySpace(whichFace, p0.x, p0.y, p0.z, &v0);
		mapOntoPolySpace(whichFace, p1.x, p1.y, p1.z, &v1);
		
#ifdef VERBOSE
		fprintf( stderr, "[X0] = %f [Y0] = %f [Z0] = %f\n", p0.x, p0.y, p0.z );
		fprintf( stderr, "[X1] = %f [Y1] = %f [Z1] = %f\n", p1.x, p1.y, p1.z );
		fprintf( stderr, "v0 [X] = %f [Y] = %f\n", v0.x, v0.y );
		fprintf( stderr, "v1 [X] = %f [Y] = %f\n", v1.x, v1.y );
#endif
		
		p.x = qCurrent.x - v0.x;
		p.y = qCurrent.y - v0.y;
		v.x = v1.x - v0.x;
		v.y = v1.y - v0.y;
		
		z[i] = v.x * p.y - p.x * v.y;
		
		if (  Negative(z[i]) )
		{
			/* find which edge is this */
			we[i] = findEdgeForVert( faces[whichFace].v[i],
									faces[whichFace].v[(i+1)%nverts],
									&(dir[i]) );
			/*
			 * The difference between 'linesIntersect' and 'goodLinesIntersect' is that
			 * 'goodLinesIntersect' does not do a bounding box test, whereas 'linesIntersect'
			 * does. I have the 2 versions since I thought that sometimes, due to
			 * rounding off, the bounding box test was giving me wrong results.
			 */
			/*it[i] = goodLinesIntersect( qPrevious, qCurrent, v0, v1, &(intersec[i]), &(tt[i]) );*/
			it[i] = linesIntersect( qPrevious, qCurrent, v0, v1, &(intersec[i]), &(tt[i]) );
		}
	}
	/*
	 * If all z are greater or equal to 0.0, then the cell
	 * is inside the polygon. Otherwise we proceed to
	 * find out which edge it crossed
	 */
	/*if ( z[0] >= 0.0 && z[1] >= 0.0 && z[2] >= 0.0 ) return -1;*/
	if ( GreaterEqualZero(z[0]) && GreaterEqualZero(z[1]) && GreaterEqualZero(z[2]) )
		return -1;
	
	else{
#ifdef VERBOSE
		/*fprintf( stderr, "qCurrent [X] = %f [Y] = %f\n", qCurrent.x, qCurrent.y );
		 fprintf( stderr, "qPrevious [X] = %f [Y] = %f\n", qPrevious.x, qPrevious.y );
		 fprintf( stderr, "\nGeometry vertices for face %d\n", whichFace );*/
		fprintf( stderr, "\nCell NOT inside polygon!\n");
#endif
		
		for(i = 0; i < nverts; i++)
		{
			if ( Negative( z[i] ) /*z[i] < 0.0*/ && it[i] == DO_INTERSECT )
			{
				
#ifdef VERBOSE
				
				fprintf( stderr, "intersec [X] = %f [Y] = %f\n", intersec[i].x, intersec[i].y );
				fprintf( stderr, "[%d] z = %f e = %d dir = %d\n", i, z[i], we[i], dir[i] );
				fprintf( stderr, "intersect = %d t = %lg\n", it[i], tt[i]);
#endif
				/*	
				 * I have to make sure here that the just found intersection
				 * is not the same one as before
				 */
				if ( prevEdge != we[i] ){
					mapFromPolySpace(whichFace, intersec[i].x, intersec[i].y, intersec3D );
					*t = tt[i];
					*direction = dir[i];
#ifdef VERBOSE
					fprintf( stderr, "2D: x = %f y = %f\n", intersec[i].x, intersec[i].y);
					fprintf( stderr, "3D: x = %f y = %f z = %f\n", intersec3D->x, intersec3D->y, intersec3D->z );
#endif
					return we[i];
				}
			}
		}
		
		/*
		 * If I have reached this point we have a problem.
		 * Before leaving the program I am printing which
		 * should be useful info
		 */
		for(i = 0; i < nverts; i++)
		{
			//fprintf( stderr, "[%d] z = %lg t = %lg intersec = %d e = %d dir = %d\n",
				//	i, z[i], tt[i], it[i], we[i],dir[i] );
			if ( we[i] != -1 ){
				//fprintf( stderr, "edge = %d pf = %d nf = %d cell = %d\n",
					//	we[i], edges[we[i]].pf, edges[we[i]].nf, c->ctype );
				/*fprintf( stderr, "matrix for next->previous face:\n");
				 printMatrix( edges[we[i]].np );
				 fprintf( stderr, "matrix for previous->next face:\n");
				 printMatrix( edges[we[i]].pn );*/
			}
		}
		//fprintf( stderr, "For face %d we have the neighboring faces:\n", whichFace );
		for(i = 0; i < faces[whichFace].nPrimFaces; i++)
		{
			//fprintf( stderr, "[%d] angle = %f\n",
				//	faces[whichFace].primFaces[i],
				//	rad2deg(faces[whichFace].rotAngles[i]) );
		}
        
		return -2;
		//errorMsg("If I reached this point I could not find an edge to move cells!");
	}
}


/*
 *------------------------------------------------------
 *	It will return TRUE if the cell is inside
 *	the polygon and FALSE otherwise. If the flag
 * 	'currentPosition' is TRUE, then we test the
 *	currentPosition of the cell; if it is FALSE
 *	we test the previous position of the cell
 *------------------------------------------------------
 */
int
cellInPoly( CELL *c, int currentPosition )
{
	
	int nverts, whichFace;
	int i;
	Point2D p, v, q, v0, v1;
	double x0, y0, z0, x1, y1, z1, z;
	
	whichFace = c->whichFace;
	nverts = faces[whichFace].nverts;
	
	if ( currentPosition )
		mapOntoPolySpace(whichFace, c->x, c->y, c->z, &q);
	else mapOntoPolySpace(whichFace, c->xp, c->yp, c->zp, &q);
	
	for(i = 0; i < nverts; i++){
		x0 = vert[faces[whichFace].v[i]].pos.x;
		y0 = vert[faces[whichFace].v[i]].pos.y;
		z0 = vert[faces[whichFace].v[i]].pos.z;
		x1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.x;
		y1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.y;
		z1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.z;
		mapOntoPolySpace(whichFace, x0, y0, z0, &v0);
		mapOntoPolySpace(whichFace, x1, y1, z1, &v1);
		
		p.x = q.x - v0.x;
		p.y = q.y - v0.y;
		
		v.x = v1.x - v0.x;
		v.y = v1.y - v0.y;
		
		z = v.x * p.y - p.x * v.y;
		
		if ( Negative(z) ){
			if (  currentPosition )
				fprintf( stderr, "\ncell in poly Z = %lg\n", z );
			else fprintf( stderr, "\ncell in poly previous Z = %lg\n", z );
			return FALSE;
		}
	}
	return TRUE;
}

/*
 *------------------------------------------------------
 *	It will return TRUE if the point(x,y) is inside
 *	the polygon and FALSE otherwise. Assumes that
 *	the (x,y) input point received is already
 * 	expressed in the polygon coordinates
 *------------------------------------------------------
 */
int
pointInPoly2D( int whichFace, double x, double y)
{
	int nverts, i;
	Point2D p, v, v0, v1;
	double x0,y0,z0,x1,y1,z1,z;
	
	nverts = faces[whichFace].nverts;
	for(i = 0; i < nverts; i++){
		x0 = vert[faces[whichFace].v[i]].pos.x;
		y0 = vert[faces[whichFace].v[i]].pos.y;
		z0 = vert[faces[whichFace].v[i]].pos.z;
		x1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.x;
		y1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.y;
		z1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.z;
		mapOntoPolySpace(whichFace, x0, y0, z0, &v0);
		mapOntoPolySpace(whichFace, x1, y1, z1, &v1);
		
		p.x = x - v0.x;
		p.y = y - v0.y;
		
		v.x = v1.x - v0.x;
		v.y = v1.y - v0.y;
		
		z = v.x * p.y - p.x * v.y;
		if ( Negative(z) /*z < 0.0*/ ) return FALSE;
	}
	return TRUE;
}

/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void
mapFromPolySpace(int whichFace, double x, double y, Point3D *p)
{
	Point3D q;
	
	q.x = x;
	q.y = y;
	q.z = 0.0;
	
	V3PreMul( &q, &(faces[whichFace].p2o));
	
	p->x = q.x;
	p->y = q.y;
	p->z = q.z;
}

/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void mapOntoPolySpace(int whichFace, double x, double y, double z, Point2D *p)
{
	Point3D q;
	
	q.x = x;
	q.y = y;
	q.z = z;
	
	V3PreMul( &q, &(faces[whichFace].o2p));
	
	p->x = q.x;
	p->y = q.y;
}



/*
 *--------------------------------------------------
 *	I wanted to make sure that the distances
 *	being computed in the 2D space of the polygon
 *	were the same as computed in the 3D original
 *	space
 *	I AM NOT USING THIS!
 *--------------------------------------------------
 */
void testDistances( void )
{
	int i;
	CELL *c;
	Point2D p, q;
	double d3D, d2D;
	
	for( i = 0; i <  NumberFaces; i++){
		c = faces[i].head->next;
		while( c != faces[i].tail ){
			mapOntoPolySpace(c->whichFace, c->x, c->y, c->z, &p);
			mapOntoPolySpace(c->whichFace, c->xp, c->yp, c->zp, &q);
			d3D = sqrt((c->x - c->xp) * (c->x - c->xp) +
					   (c->y - c->yp) * (c->y - c->yp) +
					   (c->z - c->zp) * (c->z - c->zp));
			d2D = sqrt((p.x - q.x) * (p.x - q.x) + 
					   (p.y - q.y) * (p.y - q.y));
			fprintf( stderr, "d3D = %lg d2D = %lg\n", d3D, d2D );
			c = c->next;
		}
	}
}

/*
 *------------------------------------------------------------
 *	It will return TRUE if the previous position
 *	of the cell is inside the polygon and FALSE otherwise
 * 	I AM NOT USING THIS ANYMORE (Mar. 13, 1998)
 *------------------------------------------------------------
 */
int
cellInPolyPrevious( CELL *c )
{
	
	int nverts, whichFace;
	int i;
	Point2D p, v, q, v0, v1;
	double x0,y0,z0,x1,y1,z1,z;
	
	whichFace = c->whichFace;
	nverts = faces[whichFace].nverts;
	
	mapOntoPolySpace(whichFace, c->xp, c->yp, c->zp, &q);
	
	for(i = 0; i < nverts; i++){
		x0 = vert[faces[whichFace].v[i]].pos.x;
		y0 = vert[faces[whichFace].v[i]].pos.y;
		z0 = vert[faces[whichFace].v[i]].pos.z;
		x1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.x;
		y1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.y;
		z1 = vert[faces[whichFace].v[(i+1)%nverts]].pos.z;
		mapOntoPolySpace(whichFace, x0, y0, z0, &v0);
		mapOntoPolySpace(whichFace, x1, y1, z1, &v1);
		
		p.x = q.x - v0.x;
		p.y = q.y - v0.y;
		
		v.x = v1.x - v0.x;
		v.y = v1.y - v0.y;
		
		z = v.x * p.y - p.x * v.y;
		
		if ( Negative(z) ){
			fprintf( stderr, "\ncell in poly previous Z = %lg\n", z );
			return FALSE;
		}
	}
	return TRUE;
}

/*
 *--------------------------------------------------
 *	For each cell, compute its coordinates
 *	in polygon space. Also, update the
 *	count of cells in each face, since
 *	many cells have changed faces
 *	I AM NOT USING THIS ANYMORE!!!
 *--------------------------------------------------
 */
void compCellsPolygonCoordinatesBary( void )
{
	int i;
	int totalNumberOfCells, cellsOnThisFace;
	CELL *c;
	
	totalNumberOfCells = 0;
	
	for( i = 0; i < NumberFaces; i++ ){
		cellsOnThisFace = 0;
		c = faces[i].head->next;
		while( c != faces[i].tail ){
			c->px = c->bary.w;
			c->py = c->bary.u;
			cellsOnThisFace++;
			c = c->next;
		}
		faces[i].ncells = cellsOnThisFace;
		totalNumberOfCells += cellsOnThisFace;
	}
	
	/* I want to make sure that the total number of
     cells is the same as the sum of cells per
     face */
	if ( totalNumberOfCells != NumberCells){
		fprintf( stderr, "Cells from faces = %d -- Cells = %d\n",
				totalNumberOfCells, NumberCells);
	}
}

void killCell(CELL *c){
	
	
}
