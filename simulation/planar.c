/*
 *---------------------------------------------------------
 *	planar.c
 *	All functions related with the planarization
 *---------------------------------------------------------
 */

#include <math.h>

#include "../common.h"
#include "planar.h"

/*
 *--------------------------------------------------
 *	Externally Defined Global variables
 *--------------------------------------------------
 */
extern HEAPTRI  *myheapTri;

/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void planarizeFace( int whichFace )
{
	SCELL *h, *t;

	/* head and tail nodes for the tmp list */
	h =  cellAlloc_S();
	t = cellAlloc_S();
	h->next = t;	/* mark the list is empty */

	/* transform vertices from this face */
	planarizeOneFace( whichFace, h, t );

	/* transform all primary adjacent faces */
	planarizePrimFaces( whichFace, h, t );

	/* planarize all secondary faces now */
	planarizeSecFaces( whichFace, h, t );

	/* print the tpm list of cells */
	writeTmpListCells( h, t );

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
void writeTmpListCells( SCELL *h, SCELL *t )
{
	FILE *outfile;
	SCELL *runner;

	outfile = fopen( "planarObject", "w" );
	if ( outfile == NULL )
		errorMsg("Could not open planarObject file to write!");

	runner = h->next;
	while( runner != t ){
		fprintf( outfile, "%lg %lg\n", runner->p.x, runner->p.y );
		runner = runner->next;
	}

	fclose( outfile );
}
/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void planarizePrimFaces( int whichFace, SCELL *h, SCELL *t )
{
	int i, j,  adjFace, adjEdge;
	Point3D p;
	Point2D q;
	CELL *foo = NULL;
	SCELL *copyCell;

	for( i = 0; i < faces[whichFace].nPrimFaces; i++){
		adjFace = faces[whichFace].primFaces[i];
		adjEdge = faces[whichFace].primEdges[i];
		for( j = 0; j < 3; j++ ){
			p.x = vert[faces[adjFace].v[j]].pos.x;
			p.y = vert[faces[adjFace].v[j]].pos.y;
			p.z = vert[faces[adjFace].v[j]].pos.z;
			rotatePointAroundEdge( &p, adjEdge, whichFace );
			mapOntoPolySpace( whichFace, p.x, p.y, p.z, &q );
			copyCell =  makeCopyCellSimpler( q.x, q.y, foo, FALSE );
			insertOnListOfCells_S( copyCell, h, t );
		}
	}
}
/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void planarizeOneFace( int whichFace, SCELL *h, SCELL *t )
{

	int i;
	Point3D p;
	Point2D q;
	CELL *foo = NULL;
	SCELL *copyCell;

	for( i = 0; i < 3; i++ ){
		p.x = vert[faces[whichFace].v[i]].pos.x;
		p.y = vert[faces[whichFace].v[i]].pos.y;
		p.z = vert[faces[whichFace].v[i]].pos.z;
		mapOntoPolySpace( whichFace, p.x, p.y, p.z, &q );
		copyCell = makeCopyCellSimpler( q.x, q.y, foo, TRUE );
		insertOnListOfCells_S(copyCell, h, t);
	}
}
/*
 *--------------------------------------------------------
 *--------------------------------------------------------
 */
void planarizeSecFaces( int whichFace, SCELL *h, SCELL *t )
{
	formHeapNeighFaces( whichFace );
	planarizeFacesOnHeap( whichFace, h, t );
}
/*
 *--------------------------------------------
 *--------------------------------------------
 */
void planarizeFacesOnHeap( int whichFace , SCELL *h, SCELL *t )
{

	HEAPTRI_ELEMENT *a;
	unsigned char *facesAlreadyVisited;
	int numberOfBytes;
	int whichByte, whichBit;

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
		errorMsg("Not enough space for list of visited faces! (planar.c)");

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
		/*
		 * Before processing this face I want to make sure that it
		 * was not already processed
		 */
		findByteAndBit( a->faceIndex, &whichByte, &whichBit);
		if ( !(( facesAlreadyVisited[whichByte] >> whichBit ) & 1) ){
			/*
			 * Face was not processed yet!
			 */
			planarizeSecToTmpList( whichFace, a->faceIndex, a->t, h, t );
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

}
/*
 *---------------------------------------------------------------------
 *---------------------------------------------------------------------
 */
void planarizeSecToTmpList( int whichFace, int sourceFace, Matrix4 m, SCELL *h, SCELL *t )
{
	CELL *foo = NULL;
	SCELL *copyCell;
	Point3D p;
	Point2D q;
	int i;

	for( i = 0; i < 3; i++ ){
		p.x = vert[faces[sourceFace].v[i]].pos.x;
		p.y = vert[faces[sourceFace].v[i]].pos.y;
		p.z = vert[faces[sourceFace].v[i]].pos.z;
		V3PreMul( &p, &m );
		mapOntoPolySpace( whichFace, p.x, p.y, p.z, &q );
		copyCell = makeCopyCellSimpler( q.x, q.y, foo, FALSE );
		insertOnListOfCells_S(copyCell, h, t);
	}
}
