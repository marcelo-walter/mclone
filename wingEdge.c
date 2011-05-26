/*
 *  wingEdge.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------------
 *
 *	wingEdge.c
 *	Marcelo Walter
 *	This file has the routines to build and mantain
 *	the winged-edge data structure
 *	Reference:
 *	Samet, The Design and Analysis of Spatial
 *	Data Structures
 *---------------------------------------------------------
 */

#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "wingEdge.h"

#define	VSTART	0
#define VEND	1


extern void checkIdentity( Matrix4 *a );
int **checkEdges;

double compAngleFaces(int whichEdge, int face1, int face2);

/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void checkWingEdgeFile( const char *fileName )
{
	FILE *fp;
	int nv, nf;
	
	if( (fp=fopen( fileName,"r")) == NULL){
		fprintf( stderr, "Building winged edge data structure...\n\n");
		buildWingEdge();
		saveWingEdge( fileName );
	}
	else{
		fprintf( stderr, "\n==============================================\n");
		fprintf( stderr, "Loading Winged Edge Data Structure from file %s\n", fileName );
		fprintf( stderr, "==============================================\n");
		loadWingEdge( fileName );
	}
}

/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void loadWingEdge( const char *fileName )
{
	FILE *fp;
	char *tmpStr;
	int i, j, vi, vr;
	int count;
	
	if( (fp=fopen( fileName, "r")) == NULL){
		errorMsg("Could not open winged edge file to read!");
	}
	else{
		/* make sure all faces and vertices don't have edge information */
		for( i = 0; i < NumberFaces; i++ ) faces[i].edge = -1;
		for( i = 0; i < NumberVertices; i++ ) vert[i].edge = -1;
		
		
		tmpStr = (char *) malloc( 255 );
		count = 0;
		/* Read in the vertex info */
		fgets (tmpStr, 255, fp);
		fprintf( stderr, "Processing %s", tmpStr);
		vi = NumberVertices / 10;
		vr = NumberVertices % 10;
		for( i = 0; i < vi; i++){
			for( j = 0; j < 10; j++ ){
				fscanf( fp, "%d", &(vert[i*10+j].edge) );
				/*fprintf( stderr, "Processing %d...\n", vert[i*10+j].edge);*/
				count++;
			}	
		}
		for( i = 0; i < vr; i++){
			fscanf( fp, "%d", &(vert[vi*10+i].edge) );
			/*fprintf( stderr, "Processing %d...\n", vert[vi*10+i].edge);*/
			count++;
		}
		if ( count != NumberVertices )
			errorMsg("Problem with number of vertices in the winged-edge file!");
		
		/* Read in the faces info */
		count = 0;
		fgets (tmpStr, 255, fp);
		/*fprintf( stderr, "Processing %s...\n", tmpStr);*/
		fgets (tmpStr, 255, fp);
		fprintf( stderr, "Processing %s", tmpStr);
		vi = NumberFaces / 10;
		vr = NumberFaces % 10;
		for( i = 0; i < vi; i++){
			for( j = 0; j < 10; j++ ){
				fscanf( fp, "%d", &(faces[i*10+j].edge) );
				/*fprintf( stderr, "Processing %d...\n", faces[i*10+j].edge);*/
				count++;
			}	
		}
		for( i = 0; i < vr; i++){
			fscanf( fp, "%d", &(faces[vi*10+i].edge) );
			/*fprintf( stderr, "Processing %d...\n", faces[vi*10+i].edge);*/
			count++;
		}
		if ( count != NumberFaces )
			errorMsg("Problem with number of faces in the winged-edge file!");
		
		/* read edge info */
		fgets (tmpStr, 255, fp);
		fgets( tmpStr, 255, fp );
		fscanf( fp, "%d\n", &NumberEdges );
		fprintf( stderr, "Processing %d %s", NumberEdges, tmpStr);
		
		/* get space for edges data structure */
		edges = (Edge *) malloc (sizeof (Edge) * (NumberEdges));
		if (edges == NULL) errorMsg("Problem with malloc in buildEdgesList()!");
		
		/* init all fields NULL */
		initEdgeFieldsNull();
		
		/* Finally read the edges info from file */
		for( i = 0; i < NumberEdges; i++){
			fscanf( fp, "%d %d %d %d %d %d %d %d",
				   &(edges[i].vstart),  &(edges[i].vend),
				   &(edges[i].epcw), &(edges[i].epccw),
				   &(edges[i].encw), &(edges[i].enccw),
				   &(edges[i].pf), &(edges[i].nf) );
		}
	}
	
	fclose( fp );
	free( tmpStr ); tmpStr = NULL;
	
	/* finish computing info related with the winged-edge data structure */
	completeWingEdge();
}

/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void initEdgeFieldsNull( void )
{
	
	int i;
	
	/* init all fields NULL */
	for( i = 0; i < NumberEdges; i++ ){
		edges[i].vstart = -1;
		edges[i].vend   = -1;
		edges[i].pf = -1;
		edges[i].nf = -1;
		edges[i].epcw = -1;
		edges[i].epccw = -1;
		edges[i].encw = -1;
		edges[i].enccw = -1;
	}
}
/*
 *--------------------------------------------------
 * The information saved in the .we file is:
 * For each face:
 *	- the first edge for the face
 * For each vertex:
 *	- the first edge for the vertex
 * For each edge:
 *	- vstart, vend, epcw, epccw,
 *	  encw, enccw, pf, nf 
 *	- two 4x4 rotation matrices (32 values) which
 *	  map the plane of one face into the other
 *	  (for the 2 faces which share this edge)
 *--------------------------------------------------
 */
void saveWingEdge( const char *fileName )
{
	FILE *fp;
	int i, j, vi, vr;
	
	if( (fp=fopen( fileName,"w")) == NULL){
		errorMsg("Could not open winged edge file to write!");
	}
	else{
		fprintf( stderr, "\n==============================================\n");
		fprintf( stderr, "Saving Winged Edge Data Structure on file %s\n", fileName );
		fprintf( stderr, "==============================================\n");
		/*
		 * Save vertices info
		 */
		fprintf( fp, "vertices (%d)\n", NumberVertices );
		vi = NumberVertices / 10;
		vr = NumberVertices % 10;
		for( i = 0; i < vi; i++){
			for( j = 0; j < 10; j++ ){
				fprintf( fp, "%d ", vert[i*10+j].edge );
			}
			fprintf( fp, "\n" );
		}
		for( i = 0; i < vr; i++){
			fprintf( fp, "%d ", vert[vi*10+i].edge );
		}
		if ( vr != 0 ) fprintf( fp, "\n" );
		
		/*
		 * Save faces info
		 */
		fprintf( fp, "faces (%d)\n", NumberFaces );
		vi = NumberFaces / 10;
		vr = NumberFaces % 10;
		for( i = 0; i < vi; i++){
			for( j = 0; j < 10; j++ ){
				fprintf( fp, "%d ", faces[i*10+j].edge );
			}
			fprintf( fp, "\n" );
		}
		for( i = 0; i < vr; i++){
			fprintf( fp, "%d ", faces[vi*10+i].edge );
		}
		if ( vr != 0 ) fprintf( fp, "\n" );
		
		/*
		 * Save edges info
		 */
		fprintf( fp, "edges\n" );
		fprintf( fp, "%d\n", NumberEdges );
		for( i = 0; i < NumberEdges; i++){
			fprintf( fp, "%d %d %d %d %d %d %d %d\n",
					edges[i].vstart,  edges[i].vend,
					edges[i].epcw, edges[i].epccw,
					edges[i].encw, edges[i].enccw,
					edges[i].pf, edges[i].nf );
		}
		fclose( fp );
	}
	
}

/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void buildWingEdge( void )
{
	
	int i;
	
	/* get space for auxiliary data structure that will
     help check that the object doesn't violate
     Euler's formula */
	checkEdges = allocate2DArrayInt( NumberVertices, 2 );
	
	/* make sure all faces and vertices don't have edge information */
	for( i = 0; i < NumberFaces; i++ ){
		faces[i].edge = -1;
	}
	for( i = 0; i < NumberVertices; i++ ){
		vert[i].edge = -1;
		checkEdges[i][VSTART] = 0;
		checkEdges[i][VEND] = 0;
	}
	
	/*
	 * build the edge list. Fills the vstart and vend fields of
	 * the edge list
	 */
	fprintf( stderr, "Building edge list...");
	buildEdgesList();
	fprintf( stderr, "Done!\n");
	
	/*
	 * include edge info into faces and vertices array,
	 * ie, complete for each vertex the field vert[i].edge
	 * and for each face the field faces[i].edge
	 */
	fprintf( stderr, "Updating Vertices and Faces array...");
	updateFaceVertList();
	fprintf( stderr, "Done!\n");
	
	/* finish completing edge list */
	fprintf( stderr, "Finishing completing edge list...");
	faceAdjInfo();	/* complete fields pf and nf from edge list */
	edgeAdjInfo();	/* complete fields epcw, encw, epccw, enccw from
	 edge list */
	fprintf( stderr, "Done!\n");
	
	/* make sure that the edges list has all fields complete */
	for( i = 0; i < NumberEdges; i++ ) checkInfoEdge( i );
	
	/* finish computing info related with the winged-edge data structure */
	completeWingEdge();
	
	free ( checkEdges );
	checkEdges = NULL;
}

/*
 *-----------------------------------------------------------------
 *-----------------------------------------------------------------
 */
void completeWingEdge( void )
{
	
	int i, j;
	
	/*
	 * Find out, for each vertex, first how many adjacent
	 * edges there are and then store this information
	 * with the vertex
	 */
	for( i = 0; i < NumberVertices; i++ ){
		/* the number of neighboring faces is equal to the
		 total number of edges sharing this vertex */
		vert[i].nNeighFaces = countEdgesFromVertex( i );
		/* get space for list of neighboring edges for the vertices */
		vert[i].neighFaces = (int *) malloc( sizeof ( int ) * vert[i].nNeighFaces );
		/*fprintf( stderr, "For vertex %d there are %d adj faces\n", i, vert[i].nNeighFaces );*/
	}
	
	/*
	 * Store the information
	 */
	for( i = 0; i < NumberVertices; i++ ){
		findSecAdjFaces( i );
	}
	
	/* Go over the face list to fill the adjacent faces info */
	fprintf( stderr, "Filling info for adjacent faces list...");
	for( i = 0; i < NumberFaces; i++ ) 
		findPrimAdjFaces( i );
	fprintf( stderr, "Done!\n");
	
	fprintf( stderr, "Filling info for angles for adjacent faces...");
	/* Go one more time over the face list
     to compute the angles between adjacent faces */
	for( i = 0; i < NumberFaces; i++ ){
		for( j = 0; j < faces[i].nPrimFaces; j++ ){
			faces[i].rotAngles[j] = compAngleFaces1(i, faces[i].primFaces[j]);
		}
	}
	fprintf( stderr, "Done!\n");
	
	/*
	 * Finally, compute the matrices that rotate the faces about the
	 * edges, if we haven't loaded them from a .we file
	 */
	fprintf( stderr, "Computing matrices for rotation around edges...");
	for( i = 0; i < NumberEdges; i++ ) compMatEdges( i );
	fprintf( stderr, "Done!\n");
	
	/*printEdges();*/
}
/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void compMatEdges( int whichEdge )
{
	double angle;
	Point3D v1, v2;
	Matrix4 tmp;
	
	angle = compAngleFaces(whichEdge, edges[whichEdge].pf, edges[whichEdge].nf);
	v1 = vert[edges[whichEdge].vstart].pos;
	v2 = vert[edges[whichEdge].vend].pos;
	if ( MxRotateAxis(v1, v2, angle,
					  &(edges[whichEdge].np),
					  &(edges[whichEdge].pn)) == FALSE )
		errorMsg("MxRotateAxis could not compute rotation matrices in wingedge.c!");
	
	MatMul(  &(edges[whichEdge].pn), &(edges[whichEdge].np), &tmp );
	checkIdentity( &tmp );
}
/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void checkInfoEdge( int whichEdge)
{
	if ( edges[whichEdge].vstart == -1 ){
		fprintf( stderr, "edge %d vstart not proper!\n", whichEdge );
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].vend  == -1 ){
		fprintf( stderr, "edge %d vend not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].pf == -1 ){
		fprintf( stderr, "edge %d pf not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].nf == -1 ){
		fprintf( stderr, "edge %d nf not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].epcw == -1 ){
		fprintf( stderr, "edge %d epcw not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].epccw == -1 ){
		fprintf( stderr, "edge %d epccw not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].encw == -1 ){
		fprintf( stderr, "edge %d encw not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
	if ( edges[whichEdge].enccw == -1 ){
		fprintf( stderr, "edge %d enccw not proper!\n", whichEdge);
		errorMsg("Problem in edge list!");
	}
}
/*
 *  these are the possible cases for the edge adjacency.
 *  e1.start == e2.start (E1SE2S)
 *  e1.start == e2.end   (E1SE2E)
 *  e1.end   == e2.start (E1EE2S)
 *  e1.end   == e2.end   (E1EE2E)
 */
enum{E1SE2S, E1SE2E, E1EE2S, E1EE2E};

/*
 *--------------------------------------------------
 *	Complete the info about which edges are
 *	adjacent to each edge, that is, find
 *	epcw, epccw, encw, enccw for each edge
 *--------------------------------------------------
 */
void edgeAdjInfo( void )
{
	
	int i, j;
	int vstart, vend;
	
	fprintf(stderr,"\nComputing epcw, encw,...\n");
	for( i = 0; i < NumberEdges; i++ ){
		if (!(i % 100)) fprintf(stderr, "%d ", i);
		
		vstart = edges[i].vstart;
		vend   = edges[i].vend;
		for( j = 0; j < NumberEdges; j++ ){
			if ( i != j ){	/* make sure we are not comparing the same edge */
				if ( vstart == edges[j].vstart ){
					classifyEdge(i, j, E1SE2S);
				}else if ( vstart == edges[j].vend ){
					classifyEdge(i, j, E1SE2E);
				}else if ( vend == edges[j].vstart ){
					classifyEdge(i, j, E1EE2S);
				}else if ( vend == edges[j].vend ){
					classifyEdge(i, j, E1EE2E);
				}
			}
		}
	}
	fprintf(stderr, "\n");
}
/*
 *-------------------------------------------------------
 *	Given two edges e1 and e2 find the adjacency
 *	information
 *	based on the table presented in Baumgarts paper
 *-------------------------------------------------------
 */
void classifyEdge(int e1, int e2, int type)
{
	
	switch(type){
		case E1SE2S:	/* e1.start == e2.start */
			if ( edges[e1].pf == edges[e2].nf){
				edges[e1].epcw   = e2;
				edges[e2].enccw  = e1;
			}else if (edges[e1].nf == edges[e2].pf){
				edges[e1].enccw = e2;
				edges[e2].epcw  = e1;
			}
			break;
		case E1SE2E:	/* e1.start == e2.end */
			if ( edges[e1].pf == edges[e2].pf){
				edges[e1].epcw  = e2;
				edges[e2].epccw  = e1;
			}else if (edges[e1].nf == edges[e2].nf){
				edges[e1].enccw = e2;
				edges[e2].encw = e1;
			}
			break;
		case E1EE2S:	/* e1.end == e2.start */
			if ( edges[e1].nf == edges[e2].nf){
				edges[e1].encw  = e2;
				edges[e2].enccw  = e1;
			}else if (edges[e1].pf == edges[e2].pf){
				edges[e1].epccw = e2;
				edges[e2].epcw = e1;
			}
			break;
		case E1EE2E:	/* e1.end == e2.end */
			if ( edges[e1].nf == edges[e2].pf){
				edges[e1].encw = e2;
				edges[e2].epccw = e1;
			}else if (edges[e1].pf == edges[e2].nf){
				edges[e1].epccw = e2;
				edges[e2].encw = e1;
			}
			break;
		default:
			break;
	}
}

/*
 *--------------------------------------------------
 *	Complete the info about which faces are
 *	adjacent to each edge, that is, complete the
 *	'pf' and 'nf' fields of the edge list
 *--------------------------------------------------
 */
void faceAdjInfo( void )
{
	
	int i;
	
	fprintf(stderr,"\nComputing pf and nf...\n");
	
	for( i = 0; i < NumberEdges; i++ ){
		if (!(i%100)) fprintf(stderr, "%d ", i);
		edges[i].pf  = findFace( edges[i].vstart, edges[i].vend);
		/* did I find a match? */
		if ( edges[i].pf < 0 ){
			printf("edge[%d] vstart %d vend %d\n", i, edges[i].vstart, edges[i].vend);
			errorMsg("Could not find the pf face for edge!");
		}
		edges[i].nf = findFace( edges[i].vend, edges[i].vstart);
		/* did I find a match? */
		if ( edges[i].nf < 0 ){
			printf("edge[%d] vstart %d vend %d\n", i, edges[i].vstart, edges[i].vend);
			errorMsg("Could not find the nf face for edge!");
		}
	}
	fprintf(stderr, "\n");
}

/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
int findFace( int vstart, int vend )
{
	
	int i, j;
	
	for( i = 0; i < NumberFaces; i++ ){
		for( j = 0; j < faces[i].nverts; j++){
			if ( (vstart == faces[i].v[j]) &&
				(vend   == (faces[i].v[(j + 1) % faces[i].nverts]) ) )
				return i;
		}
	}
	return -1;
}

/*
 *--------------------------------------------------
 *
 *--------------------------------------------------
 */
void updateFaceVertList( void )
{
	int i, vstart, vend, foo;
	
	fprintf(stderr, "\nProcessing face: ");
	/* update the edge info for the faces */
	for( i = 0; i < NumberFaces; i++ ){
		/* get the fisrt pair of vertices as defining the default edge
		 for this face */
		if ( !(i % 100) ) fprintf(stderr, "%d ", i);
		vstart = faces[i].v[0];
		vend = faces[i].v[1];
		faces[i].edge = findEdgeForVert( vstart, vend, &foo);
		
		/* did I find a match? */
		if ( faces[i].edge == -1 ){
			fprintf( stderr, "face %d vstart %d vend %d\n", i, vstart, vend);
			errorMsg("Could not find edge for face!");
		}
	}
	
	fprintf(stderr,"\n");
	
	/*  update the edge info for the vertices */
	for( i = 0; i < NumberVertices; i++ ){
		vert[i].edge = findEdgeForVert( i, -1, &foo);
		
		/* did I find a match ? */
		if ( vert[i].edge == -1 ){
			fprintf( stderr, "vertex %d\n", i);
			errorMsg("Could not find edge for vertex!");
		}
	}
}

/*
 *--------------------------------------------------
 *	Given 2 input vertices this routine returns
 *	the pointer into the edge list that matches
 *	the 2 vertices given.
 * 	I use this routine twice depending on the
 *	value received as 'vend'. If 'vend' is
 *	negative it means I am looking for the first
 *	edge that matches a single vertex and not
 *	a pair of vertices as is the case for faces
 *	'direction' returns TRUE if the edge found
 *	is from vstart->vend and FALSE if the edge
 *	found is from vend->vstart
 *
 *--------------------------------------------------
 */
int findEdgeForVert( int vstart, int vend, int *direction )
{
	
	int i;
	
	if ( vend >= 0 ){
		/* it means I am looking to match an edge for a face */
		for(i=0; i < NumberEdges; i++){
			if (vstart == edges[i].vstart && vend == edges[i].vend ){
				*direction = TRUE;
				return i;
			}
			if (vstart == edges[i].vend   && vend == edges[i].vstart ){
				*direction = FALSE;
				return i;
			}
		}
		return -1;
	}
	else{
		/* it means I am trying to match a single vertex with an edge */
		for(i=0; i < NumberEdges; i++){
			if (  vstart == edges[i].vstart )
				return i;
		}
		return -1;
	}
}

/*
 *----------------------------------------------------
 *	This routine fills the vstart and vend fields
 *	of the edge list
 *----------------------------------------------------
 */
void buildEdgesList( void )
{
	
	int i, j;
	int vstart, vend;
	int currentEdgeIndex, tmp;
	
	/* get space for edges data structure */
	edges = (Edge *) malloc (sizeof (Edge) * (NumberEdges));
	if (edges == NULL) errorMsg("Problem with malloc in buildEdgesList()!");
	
	/* init all fields NULL */
	for( i = 0; i < NumberEdges; i++ ){
		edges[i].vstart = -1;
		edges[i].vend   = -1;
		edges[i].pf = -1;
		edges[i].nf = -1;
		edges[i].epcw = -1;
		edges[i].epccw = -1;
		edges[i].encw = -1;
		edges[i].enccw = -1;
	}
	
	fprintf(stderr,"\nComputing edge...\n");
	
	for( i = 0; i < NumberFaces; i++ ){ /* for each face */
		for( j = 0; j < faces[i].nverts; j++){ /* for each vertex in the face */
			if (!(currentEdgeIndex % 100)) fprintf(stderr,"%d ", currentEdgeIndex);
			vstart = faces[i].v[j];
			vend = faces[i].v[(j + 1) % faces[i].nverts];
			checkEdges[vstart][VSTART]++;	
			checkEdges[vend][VEND]++;
			tmp = includeEdge( vstart, vend );
			if ( tmp != -1) currentEdgeIndex = tmp;
		}
	}
	fprintf(stderr, "\n\n");
	fprintf(stderr, "\tFinal correct number of edges = %d!\n", currentEdgeIndex + 1);
	
	/* This is the actual correct number of needed edges */
	NumberEdges = currentEdgeIndex + 1;
	
	/* check that all edges have vertices info */
	for( i = 0; i < NumberEdges; i++ ){
		if ( edges[i].vstart == -1 || edges[i].vend == -1){
			fprintf(stderr, "edge[%d] has problems vstart = %d vend = %d\n",
					i, edges[i].vstart, edges[i].vend);
			errorMsg("Problems with winged edge data structure!");
		}
	}
	
	/* check Euler's condition */
	/*for ( i = 0; i < NumberVertices; i++){
	 fprintf( stderr, "V[%d] Vstart = %d Vend = %d\n",
	 i,  checkEdges[i][VSTART], checkEdges[i][VEND]);
	 }*/
}

/*
 *--------------------------------------------------
 *	This routine tries to include in the edge
 *	list the edge specified by the 2 vertices
 *	'vstart' and 'vend'. If the edge is included
 *	the routine returns the index to the edge just
 *	included, otherwise it returns -1
 *--------------------------------------------------
 */
int includeEdge(int vstart, int vend)
{
	
	int i;
	static int currentEdge = 0;	/* keeps a pointer to the
	 current edge to be included */
	
	/* before including this edge I have to check if it's
     not already included */
	
	for( i = 0; i < currentEdge; i++ ){
		if ( vstart == edges[i].vstart && vend == edges[i].vend )
			return -1;
		if ( vstart == edges[i].vend && vend == edges[i].vstart )
			return -1;
	}
	
	/*
	 * Test bounds for number of edges. This is needed since
	 * I have first an estimate of the number of edges.
	 * If the actual number is larger than the compute one,
	 * here is where I get more memory for it.
	 * The initial estimated number of edges comes from
	 * the Euler's Formula E = F + V - 2
	 */
	if ( currentEdge >= NumberEdges  ){
		edges = (Edge *) realloc(edges, sizeof (Edge) * (currentEdge + 20));
		if (edges == NULL) errorMsg("Problem with malloc in includeEdge()!");
		fprintf(stderr, "\nGot more space for edges!");
		NumberEdges += 20;
	}
	
	/* include edge in edge list */
	edges[currentEdge].vstart = vstart;
	edges[currentEdge].vend = vend;
	
	/* update index for inclusion */
	currentEdge++;
	
	return (currentEdge - 1);
}

/*
 *--------------------------------------------------
 *	Returns the number of adjacent
 *	edges for the given face
 *--------------------------------------------------
 */
int countEdgesFromFace( int whichFace )
{
	
	int e, e0;
	int count = 0;
	
	e0 = faces[whichFace].edge;
	e = e0;
	
	do{
		count++;
		e = eccwForFace( e, whichFace);
	}while (e0 != e);
	
	return ( count );
}
/*
 *--------------------------------------------------
 *	Returns the number of adjacent
 *	edges for the given vertex
 *--------------------------------------------------
 */
int countEdgesFromVertex( int whichVertex )
{
	int e, e0, count;
	
	count = 0;
	e0  = vert[whichVertex].edge;
	e = e0;
	
	do{
		count++;
		e = eccwForVertex( e, whichVertex);
	}while (e0 != e);
	
	return( count );
}

/*
 *--------------------------------------------------
 *	Visits, for a given face, all adjacent
 *	faces which share an EDGE with the face
 *	(I am calling these faces 'primary'
 * 	Store that information into the faces
 * 	data structure
 *--------------------------------------------------
 */
void findPrimAdjFaces( int whichFace )
{
	
	int e, e0;
	int i = 0;
	
	e  = faces[whichFace].edge;
	e0 = e;
	
	do{
		faces[whichFace].primEdges[i] = e;
		if ( edges[e].pf == whichFace ) 
			faces[whichFace].primFaces[i] = edges[e].nf;	
		else 
			faces[whichFace].primFaces[i] = edges[e].pf;
		i++;
		e = eccwForFace( e, whichFace);
	}while (e0 != e);
	
	if ( i != faces[whichFace].nPrimFaces )
		errorMsg("Problem finding adjacent faces!");
}
/*
 *--------------------------------------------------
 *	Visits, for a given face, all adjacent
 *	faces which share an VERTEX with the face
 *	(I am calling these faces 'secondary'
 * 	Store that information into the vertices
 * 	data structure
 *--------------------------------------------------
 */
void findSecAdjFaces( int whichVertex )
{
	
	int e, e0, includePrevFace, includeNextFace;
	int i = 0, j;
	
	e  = vert[whichVertex].edge;
	e0 = e;
	
	/*fprintf( stderr, "For vertex %d I am visiting faces: ", whichVertex );*/
	
	do{
		includePrevFace = TRUE;
		includeNextFace = TRUE;
		for ( j = 0; j < i; j++ ){
			if ( vert[whichVertex].neighFaces[j] == edges[e].pf )
				includePrevFace = FALSE;
			if ( vert[whichVertex].neighFaces[j] == edges[e].nf )
				includeNextFace = FALSE;
		}
		if ( includePrevFace )
			vert[whichVertex].neighFaces[i] = edges[e].pf;
		else if ( includeNextFace )
			vert[whichVertex].neighFaces[i] = edges[e].nf;
		
		/*fprintf( stderr, "%d ", vert[whichVertex].neighFaces[i] );*/
		i++;
		e = eccwForVertex( e, whichVertex );
	}while (e0 != e);
	
	/*fprintf( stderr, "\n" );*/
	
	if ( i != vert[whichVertex].nNeighFaces )
		errorMsg("Problem finding adjacent faces!");
}
/*
 *---------------------------------------------------
 * 	Given two edges, this routine returns the vertex
 * 	that is shared by the 2 edges or -1 in case no
 * 	vertex is shared
 *---------------------------------------------------
 */
int
findVertex( int firstEdge, int secondEdge )
{
	
	if ( (edges[firstEdge].vstart == edges[secondEdge].vstart) || 
		(edges[firstEdge].vstart == edges[secondEdge].vend ) )
		return (edges[firstEdge].vstart);
	else if ( (edges[firstEdge].vend == edges[secondEdge].vstart) || 
			 (edges[firstEdge].vend == edges[secondEdge].vend ))
		return (edges[firstEdge].vend);
	else return -1;
}

/*
 *--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 */
int findEdgeForFaces( int face1, int face2 )
{
	
	int i, j, possibleEdge1, possibleEdge2;
	
	/*
	 * I will try to match an edge from the list of possible
	 * ones
	 */
	for( i = 0; i < faces[face1].nPrimFaces; i++ ){
		/* get one edge from the first face */
		possibleEdge1 = faces[face1].primEdges[i];
		/* go over all edges from the second face */
		for( j = 0; j < faces[face2].nPrimFaces; j++ ){
			possibleEdge2 = faces[face2].primEdges[j];
			if ( possibleEdge1 == possibleEdge2 )
				return( possibleEdge1 );
		}
	}
	/* could not find an edge. Returns -1 */
	return -1;
}

/*
 *--------------------------------------------------
 *	Given an edge and a face, return the next
 *	edge counterclockwise
 *--------------------------------------------------
 */
int eccwForFace( int whichEdge, int whichFace )
{
	if ( edges[whichEdge].pf == whichFace ) return edges[whichEdge].epccw;
	else if ( edges[whichEdge].nf == whichFace ) return edges[whichEdge].enccw;
	else return -1;
}
/*
 *--------------------------------------------------
 *	Given an edge and a face, return the next
 *	edge clockwise
 *--------------------------------------------------
 */
int ecwForFace( int whichEdge, int whichFace )
{
	if ( edges[whichEdge].pf == whichFace ) return edges[whichEdge].epcw;
	else if ( edges[whichEdge].nf == whichFace ) return edges[whichEdge].encw;
	else return -1;
}
/*
 *--------------------------------------------------
 *	Given an edge and a vertex, return the next
 *	edge counterclockwise
 *--------------------------------------------------
 */
int eccwForVertex( int whichEdge, int whichVertex )
{
	if ( edges[whichEdge].vstart == whichVertex ) return edges[whichEdge].epcw;
	else if ( edges[whichEdge].vend == whichVertex ) return edges[whichEdge].encw;
	else return -1;
}
/*
 *--------------------------------------------------
 *	Given an edge and a vertex, return the next
 *	edge clockwise
 *--------------------------------------------------
 */
int ecwForVertex( int whichEdge, int whichVertex )
{
	if ( edges[whichEdge].vstart == whichVertex ) return edges[whichEdge].enccw;
	else if ( edges[whichEdge].vend == whichVertex ) return edges[whichEdge].epccw;
	else return -1;
}

/*
 *----------------------------------------------------------------------
 *	Given the two faces, returns the angle (in radians) between
 *	them by computing the dot product between the normal vectors
 *	for each face
 *----------------------------------------------------------------------
 */
double compAngleFaces1(int face1, int face2)
{
	
	double tmp;
	
	tmp = (faces[face1].planeCoef.a * faces[face2].planeCoef.a)+
	(faces[face1].planeCoef.b * faces[face2].planeCoef.b)+
	(faces[face1].planeCoef.c * faces[face2].planeCoef.c);
	
	/*
	 * this next test has to do with rouding-off error.
	 * Angles very close to (-PI/2, PI/2) were giving
	 * wrong answers before I included the test
	 */
	if ( fabs (tmp - 1.0) < 1.0e-6 ) tmp = 1.0;
	
	return( acos( tmp ));
}

/*
 *----------------------------------------------------------------------
 *	Given the two faces, returns the angle (in radians) between
 *	them by computing the dot product between the normal vectors
 *	for each face
 *----------------------------------------------------------------------
 */
double compAngleFaces(int whichEdge, int face1, int face2)
{
	
	double tmp;
	Point3D sine;
	Point3D n1, n2;
	Point3D v0, v1, v;
	
	tmp = (faces[face1].planeCoef.a * faces[face2].planeCoef.a)+
	(faces[face1].planeCoef.b * faces[face2].planeCoef.b)+
	(faces[face1].planeCoef.c * faces[face2].planeCoef.c);
	
	/*
	 * this next test has to do with rouding-off error.
	 * Angles very close to (-PI/2, PI/2) were giving
	 * wrong answers before I included the test
	 */
	if ( fabs (tmp - 1.0) < 1.0e-6 )
		return( acos( 1.0 ) );
	
	v0.x = vert[edges[whichEdge].vstart].pos.x;
	v0.y = vert[edges[whichEdge].vstart].pos.y;
	v0.z = vert[edges[whichEdge].vstart].pos.z;
	v1.x = vert[edges[whichEdge].vend].pos.x;
	v1.y = vert[edges[whichEdge].vend].pos.y;
	v1.z = vert[edges[whichEdge].vend].pos.z;
	v.x = v1.x - v0.x;
	v.y = v1.y - v0.y;
	v.z = v1.z - v0.z;
	V3Normalize( &v );
	n1.x = faces[face1].planeCoef.a;
	n1.y = faces[face1].planeCoef.b;
	n1.z = faces[face1].planeCoef.c;
	n2.x = faces[face2].planeCoef.a;
	n2.y = faces[face2].planeCoef.b;
	n2.z = faces[face2].planeCoef.c;
	V3Cross(&n1, &n2, &sine);
	
	if ( Negative(V3Dot(&v, &sine)))
		return ( TWOPI - acos(tmp) );
	else  return( acos( tmp ));
	
	/*fprintf( stderr, "sin(%lg %lg %lg) length = %lg\n",
	 sine.x, sine.y, sine.z, rad2deg(asin(V3Length(&sine))));
	 fprintf( stderr, "edge(%lg %lg %lg) dot = %lg\n",
	 v.x, v.y, v.z, V3Dot(&v, &sine));*/
	
}

