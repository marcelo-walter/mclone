/*
 *-----------------------------------------------------------------
 *	 voronoi.c
 *-----------------------------------------------------------------
 */

#include "interfaceVoronoi.h"

#include <stdlib.h>

#include "simulation/relax.h"
#include "simulation/forces.h"
#include "control/wingEdge.h"
#include "data/Object.h"
#include "data/cellsList.h"
#include "genericUtil.h"
#include "intersect.h"

#include "viewer/drawing.h"

/*
 *--------------------------------------------------------------------
 *      External prototypes
 *--------------------------------------------------------------------
 */
/* Michael's code for computing the voronoi */
extern "C" void getVoronoi( SCELL *headCell, SCELL *tailCell,
                                voronoiType **vorList, faceType **faceList,
                                int *howManyVert );

/*
 *--------------------------------------------------------------------
 * 	Global Variables
 *--------------------------------------------------------------------
 */
flag voronoiFlag = TRUE;  /*FALSE; */
flag voronoiComputed = FALSE;
flag voronoiColorFlag = FALSE;

int NumberVoronoiVert = 0; //From Globals.h

voronoiType *vertexAux2D; //Local Aux

/*
 *--------------------------------------------------------------------
 *	This function computes the voronoi diagram for the
 *	individual faces and combines them into a single
 *	voronoi diagram.
 *
 *	The idea is to create a temporary list with all
 * 	cells that belong to a particular face together
 * 	with their neighbors. Once the voronoi is computed
 * 	I transfer the voronoi info (polygons and list of
 * 	vertices) to the particular face that the voronoi was
 * 	computed for.
 *--------------------------------------------------------------------
 */
void
computeVoronoi( void )
{
	int i;		/* faces loop counter */
	
	fprintf( stderr, "\nComputing Voronoi in (voronoi.c)...\n");
	
	for ( i = 0; i < NumberFaces; i++ ){ /* for each face */
#ifdef VERBOSE
		fprintf( stderr, "\nProcessing face %d in voronoi.c!\n", i );
#endif
#ifdef GRAPHICS
		if(  !( i % 100 ) )fprintf( stderr, "%d ", i );
#endif
		compVoronoiForFace( i );
	}
	
	/* sets the flag */
	voronoiComputed = TRUE;
	/* inform the user */
	fprintf( stderr, "\nVoronoi apparently computed fine...\n");
	
}

/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
void compVoronoiForFace( int whichFace )
{
    //printf("face:%d,\n ",  whichFace);
	SCELL *h, *t;
	int totalNumberCells;
#ifdef saveOutput
	FILE *outfile;
	SCELL *runner;
#endif
	
	/* head and tail nodes for the tmp list */
	h = cellAlloc_S();
	t = cellAlloc_S();
	h->next = t;	/* mark the list is empty */
	
	/*
	 * First we form the list
	 */
	totalNumberCells = formTmpListOfCells( whichFace, h, t );
	
#ifdef VERBOSE
	fprintf( stderr, "Done\n");
	fprintf( stderr, "Total Number of Cells (w/out dummy!) %d\n",
			totalNumberCells );
#endif
	
#ifdef saveOutput
	if ( whichFace == 0 ){
		outfile = fopen( "vorPoints", "w" );
		runner = h->next;
		while( runner != t ){
			fprintf(outfile, "%d %f %f\n", runner->toProcess, runner->p.x, runner->p.y);
			runner = runner->next;
		}
		fclose(outfile);
	}
	
	
#endif
	/*
	 * Check how many cells we have for this face. There are
	 * 2 special cases and the general case handled by
	 * Michael's code
	 */
	switch( totalNumberCells ){
		case 0:
		case 1:
			voronoiZeroOneCells( whichFace, h, t );
			break;
		default:
			/*
			 * If voronoiGeneral returns FALSE, it means that
			 * this face has zero or one only cells and therefore
			 * we handle it differently
			 */
			if ( !voronoiGeneral( whichFace, h, t, totalNumberCells ) )
				voronoiZeroOneCells( whichFace, h, t);
			break;
	}

	/*
	 * Free the space used by the tmp list of cells
	 */
	freeListOfCells_S( h, t );
	free( h ); h = NULL;
	free( t ); t = NULL;
}


/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
void
countNeighborsByType( int whichFace, int *nOfC, int *nOfD, int *nOfE, int *nOfF )
{
	int i, adjFace;
	CELL *n;
	
	*nOfC = *nOfD = *nOfE = *nOfF = 0;
	
	fprintf(stderr, "For face %d the neighbors are: ", whichFace);
	for( i = 0; i < faces[whichFace].nPrimFaces; i++){
		adjFace = faces[whichFace].primFaces[i];
		fprintf(stderr, "%d with %d cells\n", adjFace );
		n = faces[adjFace].head->next;
		while( n != faces[adjFace].tail ){
			fprintf(stderr, "Type of cell %d\n", n->ctype );
			switch( n->ctype ){
				case C:
					(*nOfC)++;
					fprintf(stderr, "one more C type! ");
					break;
				case D:
					(*nOfD)++;
					fprintf(stderr, "one more D type! ");
					break;
				case E:
					(*nOfE)++;
					fprintf(stderr, "one more E type! ");
					break;
				case F:
					(*nOfF)++;
					fprintf(stderr, "one more F type! ");
					break;
				default:
					fprintf(stderr, "Oh, oh, got uknown type of cell in voronoi.c!");
					break;
			}
			n = n->next;
		}
	}
}
/*
 *--------------------------------------------------------------------
 * get space for vertices of the voronoi face
 *--------------------------------------------------------------------
 */
void getSpaceForVorPts( int whichFace, int howMany )
{
	if ( faces[whichFace].vorPoints != NULL ){
		free ( faces[whichFace].vorPoints );
		faces[whichFace].vorPoints = NULL;
	}
	faces[whichFace].vorPoints =
    (voronoiType *) malloc( sizeof(voronoiType) * howMany );
	if ( faces[whichFace].vorPoints == NULL )
		errorMsg( "Out of space for list of voronoi points in voronoi.c!");
}
/*
 *--------------------------------------------------------------------
 * get space for voronoi polygon for this face
 *--------------------------------------------------------------------
 */
void getSpaceForVorPol( int whichFace, int howMany )
{
	
	if ( faces[whichFace].vorFacesList != NULL ){
		free( faces[whichFace].vorFacesList );
		faces[whichFace].vorFacesList = NULL;
	}
	faces[whichFace].vorFacesList = (faceType *) malloc(sizeof(faceType) * howMany);
	if ( faces[whichFace].vorFacesList == NULL )
		errorMsg( "Out of space for list of voronoi polygons in voronoi.c!");
}
/*
 *--------------------------------------------------------------------
 *	Handles the case when there are zero or only one cell
 *	for the polygon and neighbors. In these cases the voronoi
 *	polygon is the polygon itself
 *--------------------------------------------------------------------
 */
void
voronoiZeroOneCells( int whichFace, SCELL* h, SCELL *t )
{
	
	int i;
	Point2D p;
	int nOfC = -1, nOfD = -1, nOfE = -1, nOfF = -1;
	
#ifdef VERBOSE
	fprintf( stderr, "Got into the case with 1 cell per polygon for face %d!\n",
			whichFace );
#endif
	
	/* get space for voronoi polygon for this face */
	getSpaceForVorPol( whichFace, 1 );
	
	/* get space for vertices of the voronoi face */
	getSpaceForVorPts( whichFace, faces[whichFace].nverts );
	
	/* number of voronoi polygons for this face */
	faces[whichFace].nVorPol = 1;	
	faces[whichFace].vorFacesList[0].border = IN;
	
	if ( h->next == t ){
		/* this is the case where there are NO cells assigned for
		 this face. Ideally I'd have to go over all neighboring
		 faces and count the number of neighbors of each
		 type and decide how do I want to assign a 'color'
		 for this face, since the color comes from the cell's
		 type */
		/* I have to find a better site for this cell */  
		/*countNeighborsByType( whichFace, &nOfC, &nOfD, &nOfE, &nOfF );*/
		/*fprintf( stderr, "C = %d D = %d E = %d F = %d\n",
	     nOfC, nOfD, nOfE, nOfF );*/
		faces[whichFace].vorFacesList[0].site = (CELL *) NULL;
	}
	else{
		faces[whichFace].vorFacesList[0].site = /*findSite( h->next );*/
		h->next->originalCell;
	}
#ifdef GRAPHICS
	if ( voronoiColorFlag ){
		if ( h->next == t ){
			changeCellColor( faces[whichFace].vorFacesList[0].site, 0, 1, 0);
		}
		else{
			changeCellColor( faces[whichFace].vorFacesList[0].site, 1, 0, 0 ); 
		}
	}
#endif
	
	faces[whichFace].vorFacesList[0].faceSize = faces[whichFace].nverts;
    
	/*
	 * finally transfer the information
	 */
	for ( i = 0; i < faces[whichFace].nverts; i++ ){
		faces[whichFace].vorFacesList[0].vorPts[i] = i;
		mapOntoPolySpace(whichFace,
						 vert[faces[whichFace].v[i]].pos.x,
						 vert[faces[whichFace].v[i]].pos.y,
						 vert[faces[whichFace].v[i]].pos.z,
						 &p);
		faces[whichFace].vorPoints[i].x = p.x;
		faces[whichFace].vorPoints[i].y = p.y;
		faces[whichFace].vorPoints[i].inside = TRUE;
	}
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
#endif
}


/*
 *--------------------------------------------------------------------
 *	Includes 3 dummy cells very far away to make sure
 *	we have a full covering of the original polygon
 *	with the  voronoi diagram
 *	Each cell is positioned on a line connecting the
 *	geometric center of the cell and one of the vertices
 *--------------------------------------------------------------------
 */
void
includeDummyCells( int whichFace, SCELL *h, SCELL *t )
{
	/*CELL *n;*/
	SCELL *copyCell;
	Point2D center, v2D, p;
	Point3D v3D;
	int i;
	float k = 100.0;	/* this value should guarantee a cell
	 far away from the polygon */
	
	/*n = h->next->originalCell;*/
	center.x = faces[whichFace].center2D.x;
	center.y = faces[whichFace].center2D.y;
	
	/* This is a standard warning. Implementation here is dependent upon
     the fact that all faces are 3-sided!! In case this assumption
     doesn't hold anymore, this part should be reviewed
	 */
	if ( faces[whichFace].nverts != 3 )
		errorMsg("Got no triangular face in voronoi.c!");
	
	for ( i = 0; i < faces[whichFace].nverts; i++){
		v3D.x = vert[faces[whichFace].v[i]].pos.x;	
		v3D.y = vert[faces[whichFace].v[i]].pos.y;
		v3D.z = vert[faces[whichFace].v[i]].pos.z;
		mapOntoPolySpace(whichFace, v3D.x, v3D.y, v3D.z, &v2D );
		/* parametric description of the line */
		p.x = center.x * ( 1.0 - k ) + k * v2D.x;	
		p.y = center.y * ( 1.0 - k ) + k * v2D.y;
		copyCell = makeCopyCellSimpler( p.x, p.y, (CELL *)NULL, FALSE );
		insertOnListOfCells_S(copyCell, h, t);
	}
}

/*
 *---------------------------------------------------------
 *---------------------------------------------------------
 */
int
voronoiGeneral( int whichFace, SCELL *h, SCELL *t, int originalNumberCells )
{
	int k, jj, kk, cValVorCells;
	int nverts, totalNumberValidVert, newVertexIndex;
	voronoiType *points;
	faceType *facesList;
	int *tmpVertArray;
	int nIn, nOut, nBorder, nIndexZero, vertIndex;
	int totalNumberValidCells, totalNumberCells;
	
	
#ifdef VERBOSE
	fprintf( stderr, "Got into the general case for face %d with %d cells!\n",
			whichFace, originalNumberCells );
#endif
	
	includeDummyCells( whichFace, h, t );
	totalNumberCells = originalNumberCells + 3;
	
#ifdef VERBOSE
	fprintf( stderr, "Calling Michael's code...");
#endif
	
	/* call Michael's code for computing the voronoi */
	getVoronoi(h, t, &points, &facesList, &NumberVoronoiVert);
	
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
	fprintf( stderr, "Computing Voronoi for face %d with %d vertices\n",
			whichFace, NumberVoronoiVert );
#endif    
	
	NumberVoronoiVert++;		/* Michael's code returns not including
	 the first point which represents
	 a point at infinity */
	
	
	/* get space for tmp array of vertices */
	tmpVertArray = myIntMalloc( NumberVoronoiVert );
	
	/* compute for all voronoi vertices their inside/outside
     status with respect to the face they lay on */
	for (k = 0; k < NumberVoronoiVert; k++){
		tmpVertArray[k] = -1;
		/* the inside field is set to TRUE is the point is INSIDE
		 the polygon */
		points[k].inside = 
		checkInsideOutside( whichFace, points[k].x, points[k].y);
	}
	/* this is the point 'at infinity' */
	points[0].x = 0.0;
	points[0].y = 0.0;
	points[0].inside = FALSE;
	
	/*
	 * I go over all voronoi faces and eliminate from
	 * the list the ones that have an index == 0, since
	 * that means a vertex at infinity.
	 */
	nIndexZero = checkIndexZero( totalNumberCells, facesList );
	/*
	 * I have to go once over the tmp voronoi to find out how
	 * many will belong to this face. I am NOT including
	 * only the voronoi polygons which do not intersect with the
	 * face
	 */
	nIn = nOut = nBorder= 0; /* number of cells in, out, and at the border*/
	classifyVoronoiCells( totalNumberCells, whichFace, points,
						 facesList, &nIn, &nOut, &nBorder );
#ifdef VERBOSE
	fprintf( stderr, "Total original number of cells %d\n", totalNumberCells );
	fprintf( stderr, "Before checking for intersections there are:\n");
	fprintf( stderr, "%d 'in' cells, %d border cells, %d out cells and %d index0 cells\n",
			nIn, nBorder, nOut, nIndexZero );
#endif
	/* 
	 * I can now go over the cells which are marked as OUT
	 * and check if the voronoi polygon intersects
	 * at least one edge of the face. These cells will
	 * be flaged for clipping and change status to
	 * be BORDER
	 */
	includeBorderCells( totalNumberCells, whichFace, points,
					   facesList, &nOut, &nBorder );
#ifdef VERBOSE
	fprintf( stderr, "After intersec %d in cells, %d border cells, %d out cells and %d index0 cells\n",
			nIn, nBorder, nOut, nIndexZero );
#endif
	/*
	 * The total number of valid voronoi cells is the original
	 * number minus the cells which have a vertex at infinity
	 * minus the ones out
	 */
	totalNumberValidCells = totalNumberCells - nIndexZero - nOut;
	
	/* I want to check that all cells have proper status */
	if ( !checkProperStatus( totalNumberCells, facesList ) )
		errorMsg( "Status of cell unknown in voronoi.c!");
	
#ifdef VERBOSE
	fprintf( stderr, "Final number of valid cells is %d\n", totalNumberValidCells );
#endif
	
	if ( totalNumberValidCells != 0 ){
		/*
		 * Now we know how many cells we need space for
		 */
		/* clear previous space (if any) */
		getSpaceForVorPol( whichFace, totalNumberValidCells );
		
		/*
		 * Update info on how many cells this face has
		 * I have 2 independent counters now, one for the
		 * actual number of cells in a face and other for
		 * the number of voronoi cells, since these 2
		 * are not necessarily the same
		 */
		faces[whichFace].nVorPol = totalNumberValidCells;
		
		/*
		 * We go over all cells of the tmp list and transfer to the
		 * polygon list only those which belong to this polygon
		 * according to the above selection process
		 */
		cValVorCells = 0;		/* number of valid voronoi cells */
		newVertexIndex = 0;		/* help transfer the vertices */
		for( k = 0; k < totalNumberCells; k++){
			if ( facesList[k].border != OUT &&
				facesList[k].border != INDEX0 ){
				
				/* get the number of vertices for this voronoi polygon */
				if ( facesList[k].faceSize > MAX_N_VOR_VERT)
					errorMsg("Oh, oh, more voronoi vertices than allowed in voronoi.c!");
				else nverts = facesList[k].faceSize;
				
				faces[whichFace].vorFacesList[cValVorCells].faceSize = nverts;
				faces[whichFace].vorFacesList[cValVorCells].border = facesList[k].border;
				faces[whichFace].vorFacesList[cValVorCells].site = /*findSite( facesList[k].site );*/
				facesList[k].site;
#ifdef GRAPHICS
				if ( voronoiColorFlag ){
					if ( facesList[k].border == IN )
						changeCellColor( faces[whichFace].vorFacesList[cValVorCells].site,
										0.3, 0.3, 0.3 );
					else changeCellColor( faces[whichFace].vorFacesList[cValVorCells].site,
										 0.4, 0.4, 0.4 );
				}
#endif
				
				for (jj = 0; jj < nverts; jj++){
					if ( facesList[k].vorPts[jj] == 0 )
						errorMsg("Found an index of 0! in voronoi.c!");
					/*
					 * I do not want duplicate vertices on my polygon list
					 * Therefore, before including a new vertex, I have to
					 * make sure that it has not been included yet
					 */
					vertIndex = -1;
					for( kk = 0; kk < newVertexIndex; kk++){
						if ( tmpVertArray[kk] == facesList[k].vorPts[jj] ){
							vertIndex = kk;
							break;
						}
					}
					if ( vertIndex == -1 ){      /* it is a new vertex to be included */
						vertIndex = newVertexIndex;
						tmpVertArray[vertIndex] = facesList[k].vorPts[jj];
						newVertexIndex++;
					}		/* it is a vertex already included */
					faces[whichFace].vorFacesList[cValVorCells].vorPts[jj] = vertIndex;
				}
				cValVorCells++;
			}
		}
		
		/* the count of valid voronoi cells should be the same as the
		 total number found earlier */
		if ( cValVorCells != totalNumberValidCells)
			errorMsg("Oh, oh...Potential problem in voronoi.c");
		
		totalNumberValidVert = newVertexIndex;
		
		if ( totalNumberValidVert == 0 )
			errorMsg("totalNumberValidVert == 0!");
		
		/* get space for points */
		getSpaceForVorPts( whichFace, totalNumberValidVert );
		
		/*
		 * finally transfer the information from tmp array to polygon's
		 * array
		 */
		transferTmpVoronoiToPolygon( whichFace, points, tmpVertArray, totalNumberValidVert );
		
		/* clip the voronoi polygons for this face */
		newVorPolyClip( whichFace, totalNumberValidVert );
	}
	
	else{
		//fprintf(stderr, "Valid number of cells == 0 for face %d\n", whichFace);
		return FALSE;
	}
	
	/* free space used by tmp voronoi */
	free( tmpVertArray ); tmpVertArray = NULL;
	free( points ); points = NULL;
	free( facesList ); facesList=NULL;
	return TRUE;
	
}
/*
 *--------------------------------------------------------------------
 *--------------------------------------------------------------------
 */
void
transferTmpVoronoiToPolygon( int whichFace, voronoiType *points,
							int *tmpVertArray, int nValVert )
{
	int i;
	
	for ( i = 0; i < nValVert; i++ ){
		if ( tmpVertArray[i] == 0 )
			fprintf( stderr, "\n\nindex 0 included with inside status %d\n\n",
					points[tmpVertArray[i]].inside );
		faces[whichFace].vorPoints[i].x = points[tmpVertArray[i]].x;
		faces[whichFace].vorPoints[i].y = points[tmpVertArray[i]].y;
		faces[whichFace].vorPoints[i].inside = points[tmpVertArray[i]].inside;
	}
	
}

/*
 *---------------------------------------------------------------
 *	The clipping of the voronoi polygons against the faces
 *	of the object is done using the Sutherland-Hodgman idea
 *	that is, we clip each voronoi polygon against one edge
 * 	of the face at a time
 *---------------------------------------------------------------
 */
void
newVorPolyClip( int whichFace, int prevNumberVorVert )
{
	int whichVorPoly;	 	/* voronoi polygons loop counter */
	int firstEdge, currentEdge;	/* edges of the face loop counter */
	int nVorVerts;		/* number of voronoi vertices for the voronoi polygon */
	int newNumberVorVerts;	/* after clipping we'll have a new number of
	 voronoi vertices defining the voronoi polygon */
	int j;			/* vertex loop counter for the voronoi polygon */
	int k;
	int currentNumberVertices;
	
	Point2D inVrtArray[MAX_N_VOR_VERT], outVrtArray[MAX_N_VOR_VERT];
	
	currentNumberVertices = prevNumberVorVert;
	/*
	 * This loop goes over all voronoi polygons for this face
	 */
	for( whichVorPoly = 0; whichVorPoly < faces[whichFace].nVorPol; whichVorPoly++){
		/*
		 * We only want to clip the cells that are partially in
		 * I know that for these cells there is at least ONE
		 * vertex inside the object's polygon
		 */
		if( faces[whichFace].vorFacesList[whichVorPoly].border != OUT &&
		   faces[whichFace].vorFacesList[whichVorPoly].border != INDEX0 ){
			
			nVorVerts = faces[whichFace].vorFacesList[whichVorPoly].faceSize;
			
			/*fprintf( stderr, "Vor Poly with %d vertices before clipping:\n",
			 nVorVerts );*/
			/* Make a copy of the original list of vertices */
			for( j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[whichVorPoly].vorPts[j];
				inVrtArray[j].x = faces[whichFace].vorPoints[k].x;
				inVrtArray[j].y = faces[whichFace].vorPoints[k].y;
				/*fprintf( stderr, "x = %3.2f y = %3.2f\n", inVrtArray[j].x, inVrtArray[j].y );*/
			}
			/* get the first edge of this face */
			firstEdge = faces[whichFace].edge;
			currentEdge = firstEdge;
			do{
				SutherlandHodgmanPolygonClip( inVrtArray, &outVrtArray[0],
											 nVorVerts, &newNumberVorVerts,
											 currentEdge, whichFace );
				/* check if the new number of vertices is ok */
				if ( newNumberVorVerts > MAX_N_VOR_VERT )
					errorMsg( "Too many voronoi vertices in clip voronoi!");
				
				/* Before we go to the next polygon edge I have to transfer the
				 new set of vertices to the input array */
				for( j = 0; j < newNumberVorVerts; j++){
					inVrtArray[j].x = outVrtArray[j].x;
					inVrtArray[j].y = outVrtArray[j].y;
				}
				nVorVerts = newNumberVorVerts;
				/* get next edge for this face */
				currentEdge = eccwForFace( currentEdge, whichFace );
			}while( currentEdge != firstEdge );
			
			/*
			 * At this point I should have in 'outVrtArray' the new set
			 * of clipped vertices and 'newNumberVorVerts' is the
			 * number of voronoi vertices for this voronoi polygon
			 */
			
			/* update the new number of voronoi vertices for this voronoi poly */
			faces[whichFace].vorFacesList[whichVorPoly].faceSize = nVorVerts;
			faces[whichFace].vorPoints =
			(voronoiType *) realloc( faces[whichFace].vorPoints,
									sizeof( voronoiType) * ( currentNumberVertices + nVorVerts ) );
			
			if ( faces[whichFace].vorPoints == NULL )
				errorMsg( "No space left for realloc in clipVoronoi!");
			
			/*fprintf( stderr, "Vor Poly with %d vertices after clipping:\n", nVorVerts );*/
			for( j = 0; j < nVorVerts; j++){
				/*fprintf( stderr, "x = %3.2f y = %3.2f\n", outVrtArray[j].x, outVrtArray[j].y );*/
				faces[whichFace].vorPoints[currentNumberVertices].x = outVrtArray[j].x;
				faces[whichFace].vorPoints[currentNumberVertices].y = outVrtArray[j].y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[whichVorPoly].vorPts[j] = currentNumberVertices++;
			}
			
		} /* end of if for cells that are at the border */
	}/* end of loop for the voronoi polygons for this face */
}
/*
 *---------------------------------------------------------------------------
 *	Sutherland-Hodgman Polygon Clipping
 *	Adapted from Foley & vanDamm
 *	p. 128/129
 *---------------------------------------------------------------------------
 */
void
SutherlandHodgmanPolygonClip( Point2D inVrtArray[MAX_N_VOR_VERT],
							 Point2D *outVrtArray,
							 int inLength, int *outLength,
							 int whichEdge, int whichFace  )
{
	Point2D s, p;		/* start, end points of current polygon edge */
	Point2D i;		/* intersection point */
	Point2D vStart, vEnd;	/* start and end vertices of the clip edge */
	int j;		/* vertex loop counter */
	double x, y, z;	/* tmp variables */
	int vs, ve;
	
	if ( whichFace == edges[whichEdge].pf ){
		vs = edges[whichEdge].vstart;
		ve = edges[whichEdge].vend;
	}
	else{
		vs = edges[whichEdge].vend;
		ve = edges[whichEdge].vstart;
	}
	x = vert[vs].pos.x;
	y = vert[vs].pos.y;
	z = vert[vs].pos.z;
	mapOntoPolySpace( whichFace, x, y, z, &vStart );
	x = vert[ve].pos.x;
	y = vert[ve].pos.y;
	z = vert[ve].pos.z;
	mapOntoPolySpace( whichFace, x, y, z, &vEnd );
	
	*outLength = 0;	/* start with the last vertex in inVrtArray */
	
	s = inVrtArray[inLength - 1];
	for( j = 0; j < inLength; j++){
		p = inVrtArray[j];
		if ( insideEdge( p, vStart, vEnd ) ){
			if ( insideEdge( s, vStart, vEnd ) ){
				outVrtArray[*outLength].x = p.x;
				outVrtArray[*outLength].y = p.y;
				(*outLength)++;
			}
			else{
				edgeIntersect(vStart, vEnd, s, p, &i );
				outVrtArray[*outLength].x = i.x;
				outVrtArray[*outLength].y = i.y;
				(*outLength)++;
				outVrtArray[*outLength].x = p.x;
				outVrtArray[*outLength].y = p.y;
				(*outLength)++;
			}
		}
		else{
			if( insideEdge( s, vStart, vEnd ) ){
				edgeIntersect(vStart, vEnd, s, p,  &i );
				outVrtArray[*outLength].x = i.x;
				outVrtArray[*outLength].y = i.y;
				(*outLength)++;
			}
		}
		s = p;
	}	/* end for loop */
}
/*
 *---------------------------------------------------------
 *	Auxiliary routine for voronoiGeneral
 *	Go over all voronoi faces and mark
 * 	the ones that have an index == 0, since
 * 	that means a point at infinity
 *---------------------------------------------------------
 */
int
checkIndexZero( int nCells, faceType *facesList )
{
	
	int i, j, nIndexZero;
	
	/*
	 * I go over all voronoi faces and eliminate from
	 * the list the ones that have an index == 0, since
	 * that means a point at infinity.
	 */
	nIndexZero = 0;
	
	for( i = 0; i < nCells; i++){
		facesList[i].border = UNKNOW;
		for (j = 0; j < facesList[i].faceSize; j++){
			if ( facesList[i].vorPts[j] == 0 ){
				facesList[i].border = INDEX0;
				nIndexZero++;
				break;
			}
		}
	}
	return nIndexZero;
}

/*
 *---------------------------------------------------------
 *	Auxiliary routine for voronoiGeneral
 *---------------------------------------------------------
 */
void
classifyVoronoiCells( int nCells, int whichFace, voronoiType *points,
					 faceType *facesList, int *nIn, int *nOut, int *nBorder )
{
	int i, j;
	int nVertOutside;
	
	for( i = 0; i < nCells; i++){
		nVertOutside = 0;		/* will count the number of vert outside */
		
		for (j = 0; j < facesList[i].faceSize; j++){
			if ( !(points[facesList[i].vorPts[j]].inside) )
				nVertOutside++;
		}
		
		/* 
		 * According to the number of outside vertices
		 * we can classify this face (IN, OUT, or BORDER)
		 * BORDER  means:
		 *  - the cell's center is inside the polygon boundary
		 *  - some vertex that defines the voronoi polygon is
		 *    inside the polygon boundary
		 *  - some edge of the voronoi polygon intersects some edge
		 *    of the face polygon
		 */
		if ( facesList[i].border == UNKNOW ){
			if ( nVertOutside == 0 ){
				facesList[i].border = IN;
				(*nIn)++;
			}
			else if ( nVertOutside == facesList[i].faceSize ){
				facesList[i].border = OUT;
				(*nOut)++;
			}
			else{
				if ( facesList[i].site->whichFace == whichFace ){
					facesList[i].border = BORDER;
					(*nBorder)++;
				}
				else{
					facesList[i].site->whichFace = whichFace;
					facesList[i].border = BORDER;
					(*nBorder)++;
				}
			}
		}
	}
}
/*
 *---------------------------------------------------------
 *	Auxiliary routine for voronoiGeneral
 *---------------------------------------------------------
 */
void
includeBorderCells( int nCells, int whichFace, voronoiType *points,
				   faceType *facesList, int *nOut, int *nBorder )
{
	
	int i, j, vertexIn;
	double x0, y0, x1, y1;
	Point2D p;
	
	for( i = 0; i < nCells; i++){
		if ( facesList[i].border == OUT ){
			for (j = 0; j < facesList[i].faceSize; j++){
				x0 = points[facesList[i].vorPts[j%(facesList[i].faceSize)]].x;
				y0 = points[facesList[i].vorPts[j%(facesList[i].faceSize)]].y;
				x1 = points[facesList[i].vorPts[(j+1)%(facesList[i].faceSize)]].x;
				y1 = points[facesList[i].vorPts[(j+1)%(facesList[i].faceSize)]].y;
				
				vertexIn = intersect( whichFace, x0, y0, x1, y1, &p );
				if ( vertexIn != -1 ){
					facesList[i].border = BORDER;
					facesList[i].site->whichFace = whichFace;
					(*nOut)--;
					(*nBorder)++;
					break;
				}
			}
		}
	}
}
/*
 *---------------------------------------------------------
 *	Auxiliary routine for voronoiGeneral
 *---------------------------------------------------------
 */
int
checkProperStatus( int nCells, faceType *facesList )
{
	int i;
	int cOut, cIn, cBorder;
	
	cIn = cOut = cBorder = 0;
	for( i = 0; i < nCells; i++){
		if ( facesList[i].border == OUT ) cOut++;
		else if ( facesList[i].border == IN ) cIn++;
		else if ( facesList[i].border == BORDER ) cBorder++;
		else if ( facesList[i].border == UNKNOW )
			return FALSE;
	}
#ifdef VERBOSE
	fprintf( stderr, "Checking: %d in cells, %d border cells and %d out cells\n",
			cIn, cBorder, cOut );
#endif
	
	return TRUE;
}
/*
 *----------------------------------------------------------------
 *	Checks whether the point p received lies inside the
 *	clip edge or not. Inside is defined as "to the left
 *	of the clip boundary when one looks from the first
 *	vertex to the second vertex of the clip boundary"
 *----------------------------------------------------------------
 */
byte
insideEdge( Point2D t, Point2D vStart, Point2D vEnd )
{
	double z;
	Point2D p, v;
	
	p.x = t.x - vStart.x;
	p.y = t.y - vStart.y;
	
	v.x = vEnd.x - vStart.x;
	v.y = vEnd.y - vStart.y;
	
	z  = v.x * p.y - p.x * v.y;
	if ( z < 0.0 ) return FALSE;
	return TRUE;
}			  

/*
 *---------------------------------------------------
 *	It will return TRUE if the point is inside
 *	the polygon and FALSE otherwise.
 *---------------------------------------------------
 */
byte
checkInsideOutside( int whichFace, double x, double y)
{
	
	int nFaceVerts;
	int i;
	Point2D p, v, v0, v1;
	double x0, y0, z0, x1, y1, z1, z;
	
	nFaceVerts = faces[whichFace].nverts;
	
	for(i = 0; i < nFaceVerts; i++){
		x0 = vert[faces[whichFace].v[i]].pos.x;
		y0 = vert[faces[whichFace].v[i]].pos.y;
		z0 = vert[faces[whichFace].v[i]].pos.z;
		x1 = vert[faces[whichFace].v[(i+1)%nFaceVerts]].pos.x;
		y1 = vert[faces[whichFace].v[(i+1)%nFaceVerts]].pos.y;
		z1 = vert[faces[whichFace].v[(i+1)%nFaceVerts]].pos.z;
		mapOntoPolySpace(whichFace, x0, y0, z0, &v0);
		mapOntoPolySpace(whichFace, x1, y1, z1, &v1);
		
		p.x = x - v0.x;
		p.y = y - v0.y;
		
		v.x = v1.x - v0.x;
		v.y = v1.y - v0.y;
		
		z = v.x * p.y - p.x * v.y;
		if ( z < 0.0 ) return FALSE;
	}
	return TRUE;
}



/*
 *-----------------------------------------------------------------
 *-----------------------------------------------------------------
 */
void
checkClippedVorPolygons( int whichFace )
{
	int i, j, k;
	int numOutVorVert, numInVorVert, nVorVerts;
	
	for( i = 0; i < faces[whichFace].nVorPol; i++){
		if ( faces[whichFace].vorFacesList[i].border == BORDER ){
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			numOutVorVert = numInVorVert = 0;
			for (j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				if ( faces[whichFace].vorPoints[k].inside ){
					numInVorVert++;
				}
				else numOutVorVert++;
			}
			if ( numInVorVert != nVorVerts )
				
				errorMsg("Still have a voronoi polygon not clipped!");
		}
	}
}

/*
 *------------------------------------------------------
 *	This function will go over all voronoi polygons
 *	on a given face and clip them to the face
 *	Assumptions: the 2 faces are convex, ie, the
 *	polygon face and the voronoi polygon
 *
 *	I AM NOT USING THIS ANYMORE
 *	This used to be the clipping Voronoi routine
 *	but it didn't deal with some situations and
 *	therefore I've changed to the Sutherland-Hodgman
 *	clipping
 *------------------------------------------------------
 */
int
clipVoronoiPolygons( int whichFace, int prevNumberVertices )
{
	
	int i, j, k, inIndex, outIndex, knext, vorVertIndex;
	int nVorVerts, commonVertex;
	Point2D p1, p2, q1, q2;
	int lastInside, firstInside, lastOutside, firstOutside;
	int firstEdge, secondEdge;
	int numInVorVert, numOutVorVert, currentNumberVertices;
	int originalListVert[MAX_N_VOR_VERT];
	double x, y, z, x0, y0, z0, x1, y1, z1;
	Point2D v;
	
	currentNumberVertices = prevNumberVertices;
	
	for( i = 0; i < faces[whichFace].nVorPol/*ncells*/; i++){
		/*
		 * We only want to clip the cells that are partially in
		 * I know that for these cells there is at least ONE
		 * vertex inside the object's polygon
		 */
		if ( faces[whichFace].vorFacesList[i].border == BORDER ){
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			
			/*
			 * Make a copy of the original list of vertices and counts how
			 * many inside/outside vertices there are
			 */
			for (numOutVorVert = 0, numInVorVert = 0, j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				originalListVert[j] = k;
				if ( faces[whichFace].vorPoints[k].inside ){
					numInVorVert++;
				}
				else numOutVorVert++;
			}
			
			/*
			 * go over the voronoi verts until find the 2 pairs
			 * inside/outside
			 */
			lastInside = firstInside = -1;
			firstOutside = lastOutside = -1;
			firstEdge = secondEdge = -1;
			
			/*
			 * First pair
			 */
			for (j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				if ( faces[whichFace].vorPoints[k].inside ){
					lastInside = j;
					knext = faces[whichFace].vorFacesList[i].vorPts[(j+1)%nVorVerts];
					if ( !(faces[whichFace].vorPoints[knext].inside) ){
						firstOutside = (j+1)%nVorVerts;
						break;
					}
				}
			}	
			/*
			 * Second pair
			 */
			for (j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				if ( !(faces[whichFace].vorPoints[k].inside) ){
					lastOutside = j;
					knext = faces[whichFace].vorFacesList[i].vorPts[(j+1)%nVorVerts];
					if ( faces[whichFace].vorPoints[knext].inside ){
						firstInside = (j+1)%nVorVerts;
						break;
					}
				}
			}
			
			if ( firstInside == -1 || lastInside == -1 ||
				firstOutside == -1 || lastOutside == -1)
				errorMsg("Could not find inside/outside vertices for intersection in voronoi.c!");
			
			/*
			 * Now we can compute the intersection. First between the first
			 * pair
			 */
			/*fprintf( stderr, "\nTotal number of voronoi vertices = %d\n",
			 currentNumberVertices );*/
			inIndex = faces[whichFace].vorFacesList[i].vorPts[firstInside];
			outIndex = faces[whichFace].vorFacesList[i].vorPts[lastOutside];
			/*fprintf( stderr, "inIndex = %d  v0: %3.2f %3.2f outIndex = %d v1: %3.2f %3.2f\n",
			 inIndex,
			 faces[whichFace].vorPoints[inIndex].x,
			 faces[whichFace].vorPoints[inIndex].y,
			 outIndex,
			 faces[whichFace].vorPoints[outIndex].x,
			 faces[whichFace].vorPoints[outIndex].y );*/
			firstEdge = intersect( whichFace,
								  faces[whichFace].vorPoints[inIndex].x,
								  faces[whichFace].vorPoints[inIndex].y,
								  faces[whichFace].vorPoints[outIndex].x,
								  faces[whichFace].vorPoints[outIndex].y,
								  &p1 );
			/*fprintf( stderr, "First Edge = %d p1: %3.2f %3.2f\n",
			 firstEdge, p1.x, p1.y );*/
			/*
			 * Now between the second pair
			 */
			inIndex = faces[whichFace].vorFacesList[i].vorPts[lastInside];
			outIndex = faces[whichFace].vorFacesList[i].vorPts[firstOutside];
			/*fprintf( stderr, "inIndex = %d v0: %3.2f %3.2f outIndex = %d v1: %3.2f %3.2f\n",
			 inIndex,
			 faces[whichFace].vorPoints[inIndex].x,
			 faces[whichFace].vorPoints[inIndex].y,
			 outIndex,
			 faces[whichFace].vorPoints[outIndex].x,
			 faces[whichFace].vorPoints[outIndex].y );*/
			secondEdge = intersect( whichFace,
								   faces[whichFace].vorPoints[inIndex].x,
								   faces[whichFace].vorPoints[inIndex].y,
								   faces[whichFace].vorPoints[outIndex].x,
								   faces[whichFace].vorPoints[outIndex].y,
								   &p2 );
			/*fprintf( stderr, "Second Edge = %d p1: %3.2f %3.2f\n",
			 secondEdge, p2.x, p2.y );*/
			
			if ( firstEdge == -1 || secondEdge == -1){
				fprintf( stderr, "\nFirst Edge = %d Second Edge = %d\n",
						firstEdge, secondEdge );
				fprintf( stderr, "Edges for this face:\n");
				for(j = 0; j < faces[whichFace].nPrimFaces; j++){
					fprintf( stderr, "%d ", faces[whichFace].primEdges[j] );
				}
				fprintf(stderr, "\n");
				errorMsg("Could not compute intersection for voronoi and face polygons in voronoi.c!");
			}
			/*
			 * Form the internal part of the face, ie, the voronoi
			 * vertices that are totally inside the polygon
			 */
			for (vorVertIndex=0, j = firstInside; j < (firstInside + numInVorVert); j++){
				k = originalListVert[(j%nVorVerts)];
				if ( faces[whichFace].vorPoints[k].inside ){
					faces[whichFace].vorFacesList[i].vorPts[vorVertIndex++] = k;
				}
			}
			
			/*
			 * I have to form the new face 
			 */
			if ( firstEdge == secondEdge ){
				/*
				 * That means we crossed just one edge
				 */
				/*changeCellColor( faces[whichFace].vorFacesList[i].site,
				 155, 155, 155 );*/
				/* get space for 2 more points only */
				faces[whichFace].vorPoints =
				(voronoiType *) realloc( faces[whichFace].vorPoints,
										sizeof( voronoiType) * (currentNumberVertices + 2) );
				if ( faces[whichFace].vorPoints == NULL )
					errorMsg( "No space left for realloc in clipVoronoi!");
				
				/* new number of voronoi vertices for this face */
				faces[whichFace].vorFacesList[i].faceSize = numInVorVert + 2;
				
				faces[whichFace].vorPoints[currentNumberVertices].x = p2.x;
				faces[whichFace].vorPoints[currentNumberVertices].y = p2.y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[i].vorPts[vorVertIndex++] = currentNumberVertices++;
				
				faces[whichFace].vorPoints[currentNumberVertices].x = p1.x;
				faces[whichFace].vorPoints[currentNumberVertices].y = p1.y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[i].vorPts[vorVertIndex] = currentNumberVertices++;
				
			}else{
				/*
				 * That means we crossed more than one edge
				 */
				/*changeCellColor( faces[whichFace].vorFacesList[i].site,
				 80, 80, 80);*/
				commonVertex = findVertex( firstEdge, secondEdge );
				
				if ( commonVertex == -1 ){
					/* This is an error situation. I am printing debug
					 information */
					fprintf( stderr, "\nFirst Edge = %d p1: %3.2f %3.2f\n",
							firstEdge, p1.x, p1.y );
					fprintf( stderr, "Second Edge = %d p2: %3.2f %3.2f\n",
							secondEdge, p2.x, p2.y );
					fprintf( stderr, "Edges for this face:\n");
					for(j = 0; j < faces[whichFace].nPrimFaces; j++){
						/* get the two vertices that define this edge */
						x0 = vert[edges[faces[whichFace].primEdges[j]].vstart].pos.x;
						y0 = vert[edges[faces[whichFace].primEdges[j]].vstart].pos.y;
						z0 = vert[edges[faces[whichFace].primEdges[j]].vstart].pos.z;
						x1 = vert[edges[faces[whichFace].primEdges[j]].vend].pos.x;
						y1 = vert[edges[faces[whichFace].primEdges[j]].vend].pos.y;
						z1 = vert[edges[faces[whichFace].primEdges[j]].vend].pos.z;
						
						mapOntoPolySpace(whichFace, x0, y0, z0, &q1);
						mapOntoPolySpace(whichFace, x1, y1, z1, &q2);
						fprintf( stderr, "%d:  %3.2f %3.2f -> %3.2f %3.2f\n",
								faces[whichFace].primEdges[j], q1.x, q1.y, q2.x, q2.y );
					}
					errorMsg( "Do not know yet how to deal with more than one vertex in voronoi.c!");
				}
				
				faces[whichFace].vorPoints =
				(voronoiType *) realloc( faces[whichFace].vorPoints,
										sizeof( voronoiType) * (currentNumberVertices + 1 + 2) );
				if ( faces[whichFace].vorPoints == NULL )
					errorMsg( "No space left for realloc in clipVoronoi!");
				
				/* new number of voronoi vertices for this face */
				faces[whichFace].vorFacesList[i].faceSize = numInVorVert + 1 + 2;
				
				faces[whichFace].vorPoints[currentNumberVertices].x = p2.x;
				faces[whichFace].vorPoints[currentNumberVertices].y = p2.y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[i].vorPts[vorVertIndex++] = currentNumberVertices++;
				/* bring the common vertex world coordinates into the polygon space
				 coordinates */
				x = vert[commonVertex].pos.x;
				y = vert[commonVertex].pos.y;
				z = vert[commonVertex].pos.z;
				mapOntoPolySpace(whichFace, x, y, z, &v);
				
				faces[whichFace].vorPoints[currentNumberVertices].x = v.x;
				faces[whichFace].vorPoints[currentNumberVertices].y = v.y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[i].vorPts[vorVertIndex++] = currentNumberVertices++;
				
				faces[whichFace].vorPoints[currentNumberVertices].x = p1.x;
				faces[whichFace].vorPoints[currentNumberVertices].y = p1.y;
				faces[whichFace].vorPoints[currentNumberVertices].inside = TRUE;
				faces[whichFace].vorFacesList[i].vorPts[vorVertIndex++] = currentNumberVertices++;
			}
		}
	}
}

/*
 *--------------------------------------------------------------------
 *	Handles the case when the polygon has only 2 cells
 *	This cannot be handled by the general case
 *
 *	I AM NOT USING THIS!!! NOW THE 2 CELLS CASE IS BEING
 *	HANDLED BY THE GENERAL CASE, SINCE I INCLUDE 3 CELLS
 *	AT INFINITY
 *--------------------------------------------------------------------
 */
void
voronoiTwoCells( int whichFace, SCELL* h, SCELL *t )
{
	int i;
	Point2D p;
	
#ifdef VERBOSE
	fprintf( stderr, "Got into the case with 2 cells per polygon for face %d!\n",
			whichFace );
#endif
	/* get space for voronoi cell for this face */
	if ( faces[whichFace].vorFacesList != NULL ){
		free( faces[whichFace].vorFacesList );
		faces[whichFace].vorFacesList = NULL;
	}
	faces[whichFace].vorFacesList =
    (faceType *) malloc(sizeof(faceType) * 1);
	if ( faces[whichFace].vorFacesList == NULL )
		errorMsg( "Out of space for list of voronoi polygons in voronoi.c!");
	
	/* get space for vertices of the voronoi face */
	if ( faces[whichFace].vorPoints != NULL ){
		free ( faces[whichFace].vorPoints );
		faces[whichFace].vorPoints = NULL;
	}
	faces[whichFace].vorPoints =
    (voronoiType *) malloc( sizeof(voronoiType) * faces[whichFace].nverts );
	if ( faces[whichFace].vorPoints == NULL )
		errorMsg( "Out of space for list of voronoi points in voronoi.c!");
	
	faces[whichFace].nVorPol = 1;
	faces[whichFace].vorFacesList[0].border = IN;
	faces[whichFace].vorFacesList[0].site = h->next->originalCell;
	
#ifdef GRAPHICS
	if ( voronoiColorFlag )
		changeCellColor( faces[whichFace].vorFacesList[0].site, 1, 1, 0 ); 
#endif
	
	faces[whichFace].vorFacesList[0].faceSize = faces[whichFace].nverts;
    
	/*
	 * finally transfer the information
	 */
	for ( i = 0; i < faces[whichFace].nverts; i++ ){
		faces[whichFace].vorFacesList[0].vorPts[i] = i;
		mapOntoPolySpace(whichFace,
						 vert[faces[whichFace].v[i]].pos.x,
						 vert[faces[whichFace].v[i]].pos.y,
						 vert[faces[whichFace].v[i]].pos.z,
						 &p);
		faces[whichFace].vorPoints[i].x = p.x;
		faces[whichFace].vorPoints[i].y = p.y;
		faces[whichFace].vorPoints[i].inside = TRUE;
	}
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
#endif
}
/*
 *--------------------------------------------------------------------
 *	Makes a copy a cell and returns a pointer to it.
 *	The 2D coordinates of the copy are input since I want
 *	to be able to make copies which 'live' on the plane
 *	of a particular face that I am computing the Voronoi for
 *	I AM NOT USING THIS ANYMORE. NOW I HAVE A SPECIAL
 *	SIMPLER DATA STRUCTURE JUST TO COMPUTE THE VORONOI
 *--------------------------------------------------------------------
 */
CELL *makeCopyCell( double px, double py, CELL *c)
{
	CELL *copy;
	
	copy =  cellAlloc();
	copy->px = px;
	copy->py = py;
	copy->whichFace = c->whichFace;
	copy->x = c->x;
	copy->y = c->y;
	copy->z = c->z;
	return( copy );
	
}
/*
 *---------------------------------------------------
 *	This is used when I transfer the tmp list of
 *	cells built for Voronoi purposes to the face
 *	I need to find, for each tmp cell, which
 *	original cell matches
 * 	SINCE I'VE STARTED USING THE SIMPLER CELLS
 *	TMP DATA STRUCTURE, I AM NOT USING THIS ANYMORE
 *---------------------------------------------------
 */
CELL *
findSite( CELL *c )
{
	
	int i;
	CELL *runner;
	
	for ( i = 0; i < NumberFaces; i++){
		runner = faces[i].head->next;
		while ( runner != faces[i].tail){
			if ( runner->x == c->x &&
				runner->y == c->y &&
				runner->z == c->z )
				return runner;
			runner = runner->next;
		}
	}
	errorMsg("Could not find site in voronoi.c!");
}

/*
 *--------------------------------------------------------------------
 *	This function makes a list of all cells which belong
 *	to the polygon specified by 'whichFace' plus all
 *	cells that belong to the neighboring faces to 'whichFace'
 *
 *	I AM NOT USING THIS ANYMORE!
 *--------------------------------------------------------------------
 */
int
makeTmpListOfCells( int whichFace, SCELL *h, SCELL *t )
{
	
	int nCellSameFace, nCellPrimFaces, nCellSecFaces;
	
	nCellSameFace = nCellPrimFaces = nCellSecFaces = 0;
	
#ifdef VERBOSE
	fprintf( stderr, "Neighbors on same face...");
#endif
	
	nCellSameFace = includeCellsOnSameFace( whichFace, h, t );
	
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
	fprintf( stderr, "Neighbors on adjacent face ...");
#endif
	
	nCellPrimFaces = includeCellsFromPrimFaces( whichFace, h, t );
	
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
	fprintf( stderr, "Neighbors on secondary faces ...");
#endif
	
	nCellSecFaces = includeCellsFromOtherFaces( whichFace, h, t );
	
#ifdef VERBOSE
	fprintf( stderr, "Done!\n");
#endif
	
	return( nCellSameFace + nCellPrimFaces + nCellSecFaces );
}

/*
 * Compare vertex for optimizeVoronoiVertex -> qsort
 */
int CmpVertex2D(const void *elem0, const void *elem1)
{
    int *val0=(int *)elem0;
    int *val1=(int *)elem1;

    if (vertexAux2D[*val0].x>vertexAux2D[*val1].x)
    {
        return -1;
    }
    else if (vertexAux2D[*val0].x<vertexAux2D[*val1].x)
    {
        return 1;
    }
    else
    {
        if (vertexAux2D[*val0].y>vertexAux2D[*val1].y)
        {
            return -1;
        }
        else if (vertexAux2D[*val0].y<vertexAux2D[*val1].y)
        {
            return 1;
        }
		else
		{
			return 0;
		}
    }
}

/* Created by Bins - 11/07/11
 *
 *  Optimize Voronoi Vertex
 */
void optimizeVoronoiVertex( void )
{
	int i, j, k, l;
	int nVorVerts, whichFace;
	long int countNew;

	long int *sort = NULL;
	long int *exchange = NULL;

	long int vsize;

	for( whichFace = 0; whichFace < NumberFaces; whichFace++ )
    {
		vsize = 0;
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
            nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
            for (j = 0; j < nVorVerts; j++)
		    {
                faces[whichFace].vorFacesList[i].vorPtsChanged[j] = vsize;
				vsize++;
            }
        }

		vertexAux2D = (voronoiType*)malloc(sizeof(voronoiType)*vsize);

		l = 0;

		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			for (j = 0; j < nVorVerts; j++)
			{
				k = faces[whichFace].vorFacesList[i].vorPts[j];

				vertexAux2D[l] = faces[whichFace].vorPoints[k];

				l++;
			}
		}

		// Make ordenation array
		sort = (long int*)malloc( sizeof(long int)*vsize );
		for ( i = 0; i < vsize; i++)
		{
			sort[i] = i;
		}

		//fprintf( stderr, "\nSorting sort array... " );
		qsort( sort, vsize, sizeof(long int), CmpVertex2D );

		/* Optimizing sort array and creating exchange table */
		exchange = (long int*)malloc(sizeof(long int)*vsize);
		countNew = vsize;
		exchange[sort[0]] = 0;
		for ( i = 1; i < vsize; i++)
		{
			if( (vertexAux2D[sort[i-1]].x == vertexAux2D[sort[i]].x) &&
			   (vertexAux2D[sort[i-1]].y == vertexAux2D[sort[i]].y) )
			{
				exchange[sort[i]] = exchange[sort[i-1]];
				sort[i] = sort[i-1];
				countNew--;
			}
			else
			{
				exchange[sort[i]] = exchange[sort[i-1]]+1;
			}
		}

		/* Generates new array of vertices */
		faces[whichFace].vorPoints = (voronoiType*)malloc(sizeof(voronoiType)*countNew);


		faces[whichFace].vorPoints[0] = vertexAux2D[sort[0]];
		j = 1;
		for( i = 1; i < vsize; i++)
		{
			if (sort[i-1] != sort[i])
			{
				faces[whichFace].vorPoints[j] = vertexAux2D[sort[i]];
				j++;
			}
		}

		//printf("Old: %d, New: %d \n", vsize, j);

		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{

			for( j = 0; j < faces[whichFace].vorFacesList[i].faceSize; j++)
			{
				k = faces[whichFace].vorFacesList[i].vorPtsChanged[j];
				faces[whichFace].vorFacesList[i].vorPts[j] = exchange[k];

			}
		}

    }

}
