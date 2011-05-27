/*
 *  wingEdge.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------------
 *
 *	wingEdge.h
 *	Marcelo Walter
 *	The structure is adapted from the one presented
 *	in Samet, "The Design and Analysis of Spatial
 *	Data Structures"
 *---------------------------------------------------------
 */

void 	saveWingEdge( const char *fileName );
void 	checkWingEdgeFile( const char *fileName );
void 	buildWingEdge( void  );
void 	buildEdgesList( void );
int  	includeEdge(int vstart, int vend);
int  	findEdgeForVert( int vstart, int vend, int *direction );
void 	updateFaceVertList( void );
void 	faceAdjInfo( void );
int  	findFace( int vstart, int vend);
void 	classifyEdge(int i, int j, int type);
void 	edgeAdjInfo( void );
int  	countEdgesFromFace( int whichFace );
int 	countFacesFromVertex( int whichVertex );
int  	eccwForFace( int whichEdge, int whichFace );
int  	eccwForVertex( int whichEdge, int whichVertex );
int  	ecwForFace( int whichEdge, int whichFace );
int  	ecwForVertex( int whichEdge, int whichVertex );
void 	findPrimAdjFaces( int whichFace );
void 	findSecAdjFaces( int whichFace );
double 	compAngleFaces1(int face1, int face2);
void 	checkInfoEdge( int whichEdge );
void 	compMatEdges( int whichEdge );
void 	loadWingEdge( const char *fileName );
void 	completeWingEdge( void );
int 	countEdgesFromVertex( int whichVertex );
int 	findEdgeForFaces( int face1, int face2 );
int  	findVertex( int firstEdge, int secondEdge );
void 	initEdgeFieldsNull( void );
