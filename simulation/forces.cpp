/*
 *---------------------------------------------------------
 *	forces.c
 *	All functions related with the forces computation
 *---------------------------------------------------------
 */

#include "forces.h"

#include <math.h>
#include <stdlib.h>

#include "relax.h"
#include "../util/heapTri.h"
#include "../control/wingEdge.h"
#include "../data/cells.h"
#include "../data/cellsList.h"
#include "../data/Object.h"
#include "../data/Matrix4.h"
#include "../util/genericUtil.h"

/*
 *--------------------------------------------------
 *	Externally Defined Functions
 *--------------------------------------------------
 */
//extern int  findEdgeForFaces( int face1, int face2 );
//extern void copyMatrix( Matrix4 *destiny, Matrix4 source );

/*
 *--------------------------------------------------
 *	Externally Defined Global variables
 *--------------------------------------------------
 */
//extern float	adhes[HOW_MANY_TYPES][HOW_MANY_TYPES];
//extern double 	radiusRep;
//extern double  	radiusRepSquare;
//extern HEAPTRI  *myheapTri;

/* Added by Thompson Peter Lied in 16/07/2002 */
/* Anisotropy effects */
//extern double AniX, AniY, AniCommon;

//extern double Ani;

/*Add By Fabiane Queiroz at 22/09/2010
 
/*
 *--------------------------------------------------
 *	Local Defines
 *--------------------------------------------------
 */
#define IDEAL_N_NEIGHBORS	4
/*
 *--------------------------------------------------
 *	Locally Defined Global variables
 *--------------------------------------------------
 */
int totalNumOfForcesComputation = 0, nOfNeighBelowIdeal = 0;

/*
 * The initing flag flags to the relaxation
 * routine that we DO NOT want the adhesion
 * between cells to play any role at this
 * stage. It is only used in this context.
 */
flag 	initingFlag = FALSE;

//For using Vector Fiels defined in vertices
//OBS: Vector Field is defined in Faces by Default
flag verticesVectorFieldFlag = FALSE;


/*
 * -----------------------------------------------
 * ------------------------------------------------
 */

void compForcesForFace( int whichFace )
{
	Point3D p1,p2,p3,p4,p5,p6;
	Point3D q;
	Point3D v;
	Point2D v2D;
	
	double xV1,yV1,zV1;
	
	//printf("FACE: %d\n", whichFace);
	SCELL *h, *t;
	/* head and tail nodes for the tmp list */
	h =  cellAlloc_S();
	t = cellAlloc_S();
	h->next = t;	/* mark the list is empty */
	
	/*
	 * First we form the list
	 */
	int list;
	list = formTmpListOfCells( whichFace, h, t );
	
	
	/* Add by Fabiane queiroz at 24/02/2010
	 Compute orientation vector for anisotropy effects*/
	double vx,vy;
	
	if(verticesVectorFieldFlag)
	{
		if(list != 0)
		{
			
			SCELL *runner;
			
			
			runner = h->next;
			while( runner != t )
			{
				CELL *c;
				double dx,dy;
				
				c = runner->originalCell;			
				
				v.x = c->bary.u * faces[whichFace].projectedVerticeVector1.x + c->bary.v * faces[whichFace].projectedVerticeVector2.x + c->bary.w * faces[whichFace].projectedVerticeVector3.x;
				v.y = c->bary.u * faces[whichFace].projectedVerticeVector1.y + c->bary.v * faces[whichFace].projectedVerticeVector2.y + c->bary.w * faces[whichFace].projectedVerticeVector3.y;
				v.z = c->bary.u * faces[whichFace].projectedVerticeVector1.z + c->bary.v * faces[whichFace].projectedVerticeVector2.z + c->bary.w * faces[whichFace].projectedVerticeVector3.z;
				
				
				mapOntoPolySpace(whichFace, q.x, q.y, q.z, &(v2D));
				
				AniX = Ani * (v2D.x);
				AniY = Ani * (v2D.y);
				
				dx = 0; dy = 0;
				processOneCell( runner, h, t, &dx, &dy );
				runner->originalCell->fx = dx;
				runner->originalCell->fy = dy;
				
				runner = runner->next;
				
			}
			
		}else
		{
			printf("sem lista de células...\n");
		}
		
		
	}
	else if (!verticesVectorFieldFlag)
	{
		/* Add by Fabiane queiroz at 24/02/2010
		 Compute orientation vector for anisotropy effects*/
		double vx,vy;
		
		// if(faces[whichFace].endOfProjectVector.x !=0 && faces[whichFace].endOfProjectVector.y!=0){
		
		vx = faces[whichFace].endOfProjectVector.x - faces[whichFace].centerProjecVector.x;
		vy = faces[whichFace].endOfProjectVector.y - faces[whichFace].centerProjecVector.y;
		
		// }else{
		//  vx = 0;
		//  vy = 0;
		// }
		
		AniX = Ani * vx;
		AniY = Ani * vy;
		
		// printf("vector: (%f,%f)\n", vx, vy);
		// fprintf(stderr,"AniX = %f  AniY = %f \n", AniX, AniY );
		/*
		 * Then we process the cells on the list
		 */
		processCellsInTmpList( h, t );
	}
	
	/*
	 * Free the space used by the tmp list of cells
	 */
	freeListOfCells_S( h, t );
	free( h ); h = NULL;
	free( t ); t = NULL;
}


/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
int formTmpListOfCells( int whichFace, SCELL *h, SCELL *t )
{
	int nCellSameFace, nCellPrimFaces, nCellSecFaces;
	
	nCellSameFace = nCellPrimFaces = nCellSecFaces = 0;
	
	/* Include cells on the same face */
	nCellSameFace = includeCellsOnSameFace( whichFace, h, t );
	
	/* Include cells from primary adjacent faces */
	nCellPrimFaces = includeCellsFromPrimFaces( whichFace, h, t );
	
	/* Include cells from all other faces which should be included */
	nCellSecFaces = includeCellsFromOtherFaces( whichFace, h, t );
	
	/* print tmp list */
	/*printTmpListCells( whichFace, h, t );*/
	
	return( nCellSameFace + nCellPrimFaces + nCellSecFaces );
}

/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
int includeCellsFromOtherFaces( int whichFace, SCELL *h, SCELL *t )
{
	
	int nCellOtherFaces;
	
	formHeapNeighFaces( whichFace );
	nCellOtherFaces = processHeapNeighFaces( whichFace, h, t );
	return nCellOtherFaces;
	
}
/*
 *--------------------------------------------
 *--------------------------------------------
 */
void formHeapNeighFaces( int whichFace )
{
	/*
	 * Now we can include in the heap the secondary
	 * adjacent faces. They all have by definition
	 * distance zero from current face
	 */
	insertSecAdjFacesInHeap( whichFace );
}

/*
 *--------------------------------------------
 *--------------------------------------------
 */
void insertSecAdjFacesInHeap( int whichFace )
{
	int i, j, nSecFaces, nSecFacesCW, nSecFacesCCW, parity;
	int whichVertex;
	Matrix4 t;
	int listOfSecFacesCCW[128], listOfSecFacesCW[128];
	int listOfSecEdgesCCW[128], listOfSecEdgesCW[128];
	
#ifdef VERBOSE
	fprintf( stderr, "\nStarted including sec faces on heap for face %d...\n", whichFace );
#endif
	
	/* Include secondary adjacent faces */
	for(i = 0; i < 3; i++){	/* only 3 vertices for each face */
		whichVertex = faces[whichFace].v[i];
		
#ifdef VERBOSE
		fprintf( stderr, "vertex = %d\n", whichVertex);
#endif
		
		nSecFaces = vert[whichVertex].nNeighFaces - 3;
		if ( nSecFaces == 0 ){	/* do nothing */
			nSecFacesCCW = 0;
			nSecFacesCW = 0;
		}
		/*
		 * For vertices with an odd number of adjacent faces
		 * parity is 2 (these faces have an even number of
		 * sec faces); with an even number of adjacent faces
		 * parity it is 1 (these have an odd number of sec faces)
		 */
		else if ( (nSecFaces%2) == 0.0 ){
			/*
			 * We have an even number of secondary faces. We split half the faces
			 * to the CCW path and half the faces to the CW path
			 */
			nSecFacesCCW = nSecFaces/2;
			nSecFacesCW = nSecFacesCCW;
			parity = 2;
		}
		else{
			/*
			 * We have an odd number of secondary faces. We split half+1 faces
			 * to the CCW path and half-1 faces to the CW one
			 */
			nSecFacesCCW = (float)nSecFaces/2.0 + 0.5;
			nSecFacesCW = nSecFaces - nSecFacesCCW;
			parity = 1;
		}
		
#ifdef VERBOSE
		fprintf( stderr, "nSecFaces = %d nSecFacesCCW = %d nSecFacesCW = %d\n",
				nSecFaces, nSecFacesCCW, nSecFacesCW );
#endif
		/* get both the list of secondary faces and edges for the CCW faces */
		if ( nSecFacesCCW != 0 ){
			formListOfSecFaces( whichVertex, whichFace, nSecFacesCCW, listOfSecFacesCCW, CCW, parity );
			formListOfSecEdges( listOfSecFacesCCW, nSecFacesCCW, listOfSecEdgesCCW );
			/* Include in the heap the faces from the CCW ordering */
			for ( j = 0; j < nSecFacesCCW; j++ ){
				compUnfoldingMatrix( j, nSecFacesCCW+2, listOfSecFacesCCW, listOfSecEdgesCCW, &t );
				insertFaceInHeap(listOfSecFacesCCW[j], listOfSecEdgesCCW[j], whichVertex, 0.0, t );
#ifdef VERBOSE
				fprintf( stderr, "sec face CCW %d edge %d\n",
						listOfSecFacesCCW[j],
						listOfSecEdgesCCW[j] );
#endif
			}
		}
		/* and now for the CW faces */
		if ( nSecFacesCW != 0 ){
			formListOfSecFaces( whichVertex, whichFace, nSecFacesCW, listOfSecFacesCW, CW, parity );
			formListOfSecEdges( listOfSecFacesCW, nSecFacesCW, listOfSecEdgesCW );
			/* Include in the heap the faces from the CW ordering */
			for ( j = 0; j < nSecFacesCW; j++ ){
				compUnfoldingMatrix( j, nSecFacesCW+2, listOfSecFacesCW, listOfSecEdgesCW, &t );
				insertFaceInHeap( listOfSecFacesCW[j], listOfSecEdgesCW[j], whichVertex, 0.0, t );
#ifdef VERBOSE
				fprintf( stderr, "sec face CW %d edge %d\n",
						listOfSecFacesCW[j],
						listOfSecEdgesCW[j] );
#endif
			}
		}
	}
}
/*
 *---------------------------------------------------------------------
 *	This routine computes
 *	an unfolding transformation matrix which brings a point
 *	from the planar space of currentSecFace to the planar space
 *	of the face in question. This mapping is done through a sucession
 *	of rotations around edges of common faces
 *---------------------------------------------------------------------
 */
void compUnfoldingMatrix( int currentSecFace, int nSecFaces,
						 int *listOfSecFaces, int *listOfSecEdges, Matrix4 *t )
{
	Matrix4 result;
	int i, whichEdge;
	
	loadIdentity( &result );
	loadIdentity( t );
	
	for ( i = currentSecFace; i < nSecFaces - 1; i++){
		whichEdge = listOfSecEdges[i];
		if ( edges[whichEdge].pf == listOfSecFaces[i] ){
			MatMulCum( &(edges[whichEdge].pn), &result );
			
		}
		else if ( edges[whichEdge].nf == listOfSecFaces[i] ){
			MatMulCum( &(edges[whichEdge].np), &result );
		}
		else errorMsg(" Could not find face for edge! (forces.c)" );
	}
	
	copyMatrix( t, result );
}
/*
 *--------------------------------------------
 *--------------------------------------------
 */
int processHeapNeighFaces( int whichFace , SCELL *h, SCELL *t )
{
	
	HEAPTRI_ELEMENT *a;
	unsigned char *facesAlreadyVisited;
	int numberOfBytes, nCells = 0;
	int whichByte, whichBit;
	
#ifdef VERBOSE
	fprintf( stderr, "Start processing heap for face %d\n",
			whichFace );
#endif
	/*
	 * I am keeping a flag for each visited face, since I do not
	 * want to process the same face twice. These flags are
	 * the bits of the array 'facesAlreadyVisited'.
	 * 'numberOfBytes' is the necessary number of bytes
	 * to store that information
	 */
	numberOfBytes = ((float) NumberFaces / 8.0) + 0.5;
	
	/* Get space for the visited faces array */
	facesAlreadyVisited = (unsigned char *) calloc(numberOfBytes, sizeof(unsigned char));
	if ( facesAlreadyVisited == NULL )
		errorMsg("Not enough space for list of visited faces on forces.c!");
	
	/*
	 * I want to make sure that the original face and the primary adjacent
	 * ones are not processed twice, so I mark them as processed
	 * (these were included in the tmp list before already)
	 */
	markFaceAsVisited( whichFace, facesAlreadyVisited );
	markPrimFacesAsVisited( whichFace, facesAlreadyVisited );
	
	/*
	 * We process the faces til the giraffes come home, ops,
	 * I mean, til there are no more faces in it
	 */
	while( myheapTri->size > 0 ){
		/* remove one face from the heap */
		a = HeapRemoveTri( myheapTri );
#ifdef VERBOSE
		fprintf( stderr, "Removed face %d ", a->faceIndex );
#endif
		/*
		 * Before processing this face I want to make sure that it
		 * was not already processed
		 */
		findByteAndBit( a->faceIndex, &whichByte, &whichBit);
		if ( !(( facesAlreadyVisited[whichByte] >> whichBit ) & 1) ){
#ifdef VERBOSE
			fprintf( stderr, "and processed\n");
#endif
			/*
			 * Face was not processed yet!
			 */
			nCells += insertCellsFromFaceInTmpList( whichFace, a->faceIndex, a->t, h, t );
			/* mark face as visited */
			markFaceAsVisited( a->faceIndex, facesAlreadyVisited );
		}
		/* The heap remove routine allocates space for
		 a heap node. I have to free it here */
		free( a );
	}
	
	/* Empty the heap. Maybe I don't need this here...*/
	myheapTri = ClearHeapTri( myheapTri );
	/* free space used by list of visited faces */
	free(facesAlreadyVisited);
	facesAlreadyVisited=NULL;
	
	/* return the total number of cells from all secondary faces */
	return nCells;
}

/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
int insertCellsFromFaceInTmpList( int whichFace, int sourceFace, Matrix4 m, SCELL *h, SCELL *t )
{
	CELL *c;
	SCELL *copyCell;
	Point3D p;
	Point2D q;
	int nCells = 0;
	
	c = faces[sourceFace].head->next;
	while( c != faces[sourceFace].tail ){
		p.x = c->x;
		p.y = c->y; 
		p.z = c->z;
		V3PreMul( &p, &m );
		mapOntoPolySpace( whichFace, p.x, p.y, p.z, &q );
		copyCell = makeCopyCellSimpler( q.x, q.y, c, FALSE );
		insertOnListOfCells_S(copyCell, h, t);
		nCells++;
		c = c->next;
	}
	return nCells;
}
/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
void markFaceAsVisited( int whichFace, unsigned char *facesAlreadyVisited )
{
	int whichByte, whichBit;
	
	findByteAndBit( whichFace, &whichByte, &whichBit );
	facesAlreadyVisited[whichByte] |= ( 1 << whichBit );
	
}

/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
void markPrimFacesAsVisited( int whichFace, unsigned char *facesAlreadyVisited )
{
	int whichByte, whichBit;
	int i, adjFace;
	
	for( i = 0; i < faces[whichFace].nPrimFaces; i++ ){
		adjFace = faces[whichFace].primFaces[i];
		findByteAndBit( adjFace, &whichByte, &whichBit );
		facesAlreadyVisited[whichByte] |= ( 1 << whichBit );
	}
}
/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
int includeCellsOnSameFace( int whichFace, SCELL *h, SCELL *t )
{
	
	CELL *c;
	SCELL *copyCell;
	int nCellsThisFace = 0;
	
	c = faces[whichFace].head->next;
	while( c != faces[whichFace].tail ){
		/*
		 * The TRUE flags that this cells will be processed
		 * later
		 */
		copyCell = makeCopyCellSimpler( c->px, c->py, c, TRUE );
		insertOnListOfCells_S(copyCell, h, t);
		c = c->next;
		nCellsThisFace++;
	}
	return nCellsThisFace;
}
/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
int includeCellsFromPrimFaces( int whichFace, SCELL *h, SCELL *t )
{
	
	int i, adjFace, adjEdge;
	CELL *n;
	SCELL *copyCell;
	Point3D r;
	Point2D q;
	int nCellsPrimFaces = 0;
	
	for( i = 0; i < faces[whichFace].nPrimFaces; i++){
		adjFace = faces[whichFace].primFaces[i];
		adjEdge = faces[whichFace].primEdges[i];
		/* get the first neighboring point */
		n = faces[adjFace].head->next;
		while( n != faces[adjFace].tail ){
			rotateCellAroundEdge( &r, adjEdge, whichFace, n );
			mapOntoPolySpace( whichFace, r.x, r.y, r.z, &q );
			copyCell =  makeCopyCellSimpler( q.x, q.y, n, FALSE );
			insertOnListOfCells_S( copyCell, h, t );
			n = n->next;
			nCellsPrimFaces++;
		}
	}
	return nCellsPrimFaces;
}

/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
SCELL *makeCopyCellSimpler( double px, double py, CELL *c, int process )
{
	SCELL *copy;
	
	copy =  cellAlloc_S();
	copy->p.x = px;
	copy->p.y = py;
	copy->originalCell = c;
	copy->toProcess = process;
	return( copy );
	
}

/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void processCellsInTmpList( SCELL *h, SCELL *t )
{
	
	SCELL *beingProcessed;
	double dx, dy;
	
	beingProcessed = h->next;
	while( beingProcessed != t){
		if ( beingProcessed->toProcess ){
			dx = 0; dy = 0;
			processOneCell( beingProcessed, h, t, &dx, &dy );
			beingProcessed->originalCell->fx = dx;
			beingProcessed->originalCell->fy = dy;
		}
		beingProcessed = beingProcessed->next;
	}
}

/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void processOneCell( SCELL *s, SCELL *h, SCELL *t, double *dx, double *dy )
{
	
	SCELL *curNeigh;
	CELL *c, *n;
	double fx, fy;
	int nNeighbors = 0;
	
	c = s->originalCell;
	curNeigh = h->next;
	
	while( curNeigh != t ){
		if ( s != curNeigh ){	/* I do not want to process the cell with itself */
			n = curNeigh->originalCell;
			force(s->p, curNeigh->p, c->ctype, n->ctype, &fx, &fy);
			*dx += fx;
			*dy += fy;
			nNeighbors++;
		}
		curNeigh = curNeigh->next;
	}
	
	if ( nNeighbors < IDEAL_N_NEIGHBORS )
		nOfNeighBelowIdeal++;
	
	//if(*dx != 0.0 && *dy != 0.0)  
	//  printf("dix = %f, diy = %f \n", *dx, *dy);
}




/*
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */
void insertFaceInHeap( int faceIndex, int edgeIndex, int vertIndex,
					  double dist, Matrix4 t )
{
#if 0
	HEAPTRI_ELEMENT *a;
	
	a = allocateHeapTriElem();
	a->faceIndex = faceIndex;
	a->edgeIndex = edgeIndex;
	a->vertIndex = vertIndex;
	a->distance = dist;
	copyMatrix( &(a->t), t );
	HeapInsertTri(myheapTri, a);
	free(a);
#endif
	
	HEAPTRI_ELEMENT a;
	
	a.faceIndex = faceIndex;
	a.edgeIndex = edgeIndex;
	a.vertIndex = vertIndex;
	a.distance = dist;
	copyMatrix( &(a.t), t );
	HeapInsertTri(myheapTri, &a);
	
}

/*
 *--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 */
void mapToPlane( Point3D *r, int currentSecFace, int *listOfSecFaces,
				int *listOfSecEdges, CELL *n )
{
	int i;
	Point3D p;
	
	/* Get the cell's position */
	p.x = n->x;
	p.y = n->y;
	p.z = n->z;
	
	for( i = currentSecFace; i > 0; i--){
		rotatePointAroundEdge( &p, listOfSecEdges[i-1], listOfSecFaces[i-1] );
	}
	
	r->x = p.x;
	r->y = p.y;
	r->z = p.z;
}

/*
 *--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 */
void rotatePointAroundEdge( Point3D *p, int whichEdge, int whichFace )
{
	
	if ( edges[whichEdge].nf == whichFace)
		V3PreMul( p, &(edges[whichEdge].pn));
	else if ( edges[whichEdge].pf == whichFace )
		V3PreMul( p, &(edges[whichEdge].np));
	else{
		fprintf( stderr, "face %d edge %d\n", whichFace, whichEdge );
		errorMsg("Could not find a face in rotatePointAroundEdge (relax.c)!");
	}
}

/*
 *--------------------------------------------------------------------------
 * Given a vertex and a face, this routine returns a list of
 * ordered secondary faces. The order is around the vertex
 * We divide the faces into 2 lists, each one with half the
 * number of original faces (+/- one depending if the original
 * number of faces is even or odd
 *--------------------------------------------------------------------------
 */
void formListOfSecFaces( int whichVertex, int whichFace, int nSecFaces,
						int *list, int type, int parity )
{
	int i, j, index, indexForFace;
	
	/*
	 * First we have to find the index in the vertices array
	 * for the face in question
	 */
	for ( i = 0; i < vert[whichVertex].nNeighFaces; i++ ){
		if ( vert[whichVertex].neighFaces[i] == whichFace ){
			indexForFace = i; 
			break;
		}
	}
	/*
	 * We have two different cases
	 */
	switch( type ){
		case CCW:
			/*
			 * For vertices with an odd number of adjacent faces
			 * parity is 2; with an even number it is 1
			 */
			for ( i = 0; i < nSecFaces + 2 ; i++ ){
				index = ( i + indexForFace + nSecFaces + parity ) % vert[whichVertex].nNeighFaces;
				list[i] = vert[whichVertex].neighFaces[index];
			}
			break;
		case CW:
			for ( i = nSecFaces + 1, j = 0; i >= 0; i--, j++ ){
				index = ( i + indexForFace ) % vert[whichVertex].nNeighFaces;
				list[j] = vert[whichVertex].neighFaces[index];
			}
			break;
		default:
			fprintf( stderr, "type received = %d\n", type );
			errorMsg("Wrong case in formListOfSecFaces (forces.c)!");
			break;
	}
}

/*
 *--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 */
void formListOfSecEdges( int *listOfSecFaces, int nSecFaces , int *list )
{
	int i, edge;
	
	for( i = 0; i < nSecFaces + 1; i++ ){
		edge = findEdgeForFaces( listOfSecFaces[i], listOfSecFaces[i+1] );
		if ( edge != -1 )
			list[i] = edge;
		else{
			fprintf( stderr, "faces %d and %d\n",
					listOfSecFaces[i], listOfSecFaces[i+1] );
			errorMsg( "Could not find edge between faces (relax.c)!" );
		}
	}
}

/*
 *--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 */
void rotateCellAroundEdge( Point3D *r, int whichEdge, int whichFace, CELL *c )
{
	r->x = c->x;
	r->y = c->y;
	r->z = c->z;
	
	if ( edges[whichEdge].nf == whichFace)
		V3PreMul( r, &(edges[whichEdge].pn));
	else if ( edges[whichEdge].pf == whichFace )
		V3PreMul( r, &(edges[whichEdge].np));
	else errorMsg("Could not find a face in rotateCellAroundEdge (relax.c)!");
}




/*
 *----------------------------------------
 *----------------------------------------
 */
double linearForceFunction( double distance )
{
	return( fabs(1.0 - (distance/radiusRep)));
}
/*
 *---------------------------------------------------------------------------
 *---------------------------------------------------------------------------
 */
void findByteAndBit( int faceIndex, int *byte, int *bit )
{
	*byte = faceIndex/8.0;
	*bit = faceIndex%8;
}

/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void force( Point2D p, Point2D q,CELL_TYPE ptype, CELL_TYPE qtype,double *dix, double *diy)
{
	double d, dsquare, dx, dy;
	double adh, f;
	double tmpx, tmpy;
	
	
	*dix = 0.0;
	*diy = 0.0;
	
	dx = p.x - q.x;
	dy = p.y - q.y;
	
	/*
	 * This should take care when the 2 cells share the
	 * same position
	 */
	if ( Zero( dx ) && Zero( dy ) )
	{
		if (adhes[ptype][qtype] >= 1.0)
			adh = adhes[ptype][qtype];
		else
			adh = 1.0 - adhes[ptype][qtype];
		*dix = adh;
		*diy = adh;
		return;
	}
	
	else
	{
		totalNumOfForcesComputation++;
		dsquare = dx*dx + dy*dy;
		
		/*
		 * We don't want cells that are further than the repulsive
		 * radius distance
		 */
		if( dsquare > radiusRepSquare )
			return;
		
		d = sqrt( dsquare );
		
		dx /= d;
		dy /= d;
		
		f = linearForceFunction( d );
		
		if ( initingFlag )
		/*
		 * The idea here is that I am initing the pattern
		 * it means that we want the cells to drift apart
		 * from each other independent of type
		 */
			adh = 1.0 - adhes[C][C];
		else
			adh = 1.0 - adhes[ptype][qtype];
		
		
		*dix = f * adh * dx * radiusRep;
		*diy = f * adh * dy * radiusRep;
		// printf("\nnew: (%f,%f) old: (%f,%f) ", *dix, *diy, dx, dy);
		/* Added by Thompson Peter Lied in 15/07/20002 */
		tmpx = *dix;
		tmpy = *diy;
		
		
		/* compute new position
		 include anisotropy effect */
		/*apply anisotropy in C cell only*/
		//if(qtype != 0){
		//AniX = 0; AniY = 0;
		// }
		
		*dix = AniX + (AniCommon * tmpx);
		*diy = AniY + (AniCommon * tmpy);
	    /* end */
		
	}
	
	
}
