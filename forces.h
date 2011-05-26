/*
 *  forces.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-------------------------------------------------
 *	forces.h
 *-------------------------------------------------
 */

enum{CW, CCW};


double 	linearForceFunction( double distance );
void 	force( Point2D p, Point2D q,
			  CELL_TYPE ptype, CELL_TYPE qtype,
			  double *dix, double *diy );
void 	mapToPlane( Point3D *r, int currentSecFace, int *listOfSecFaces,
				   int *listOfSecEdges, CELL *n );
void 	rotateCellAroundEdge( Point3D *r, int whichEdge,
							 int whichFace, CELL *c );
void 	rotatePointAroundEdge( Point3D *r, int whichEdge, int whichFace );
void 	formListOfSecFaces( int whichVertex, int whichFace, int nSecFaces,
						   int *list, int type, int parity );
void 	formListOfSecEdges( int *listOfSecFaces, int nSecFaces , int *list );
void 	insertSecAdjFacesInHeap( int whichFace );
void 	findByteAndBit( int faceIndex, int *byte, int *bit );
void 	insertFaceInHeap(int faceIndex, int edgeIndex,
						 int vertIndex, double dist, Matrix4 t );
SCELL 	*makeCopyCellSimpler( double px, double py, CELL *c, int process );
int	includeCellsOnSameFace( int whichFace, SCELL *h, SCELL *t );
int	includeCellsFromPrimFaces( int whichFace, SCELL *h, SCELL *t );
int 	includeCellsFromOtherFaces( int whichFace, SCELL *h, SCELL *t );
int	formTmpListOfCells( int whichFace, SCELL *h, SCELL *t );
void 	compForcesForFace( int whichFace );
void 	processCellsInTmpList( SCELL *h, SCELL *t );
void 	processOneCell( SCELL *s, SCELL *h, SCELL *t, double *dx, double *dy );
void 	formHeapNeighFaces( int whichFace );
int	processHeapNeighFaces( int whichFace, SCELL *h, SCELL *t);
void 	markFaceAsVisited( int whichFace, unsigned char *facesAlreadyVisited );
void 	markPrimFacesAsVisited( int whichFace, unsigned char *facesAlreadyVisited );
void 	compUnfoldingMatrix( int currentSecFace, int inSecFaces,
							int *listOfSecFaces, int *listOfSecEdges,
							Matrix4 *t );
int 	insertCellsFromFaceInTmpList( int whichFace, int sourceFace, Matrix4 m,
									 SCELL *h, SCELL *t );
void 	planarizeFace( int whichFace );
void 	writeTmpListCells( SCELL *h, SCELL *t );
void 	planarizePrimFaces( int whichFace, SCELL *h, SCELL *t );
void 	planarizeOneFace( int whichFace, SCELL *h, SCELL *t );
void 	planarizeSecFaces( int whichFace, SCELL *h, SCELL *t );
void 	planarizeFacesOnHeap( int whichFace , SCELL *h, SCELL *t );
void 	planarizeSecToTmpList( int whichFace, int sourceFace, Matrix4 m, SCELL *h, SCELL *t );

/*add by Fabiane Queiroz*/
void projectVerticesVectors(int , Point3D*, Point3D* , Point3D* );


/* I am not using these anymore */
int  compForcesOnSecFaces( Point2D cellPos2D, CELL *c,
						  double *sx, double *sy );
int  compForcesOnSameFace( Point2D cellPos2D, CELL *c,
						  double *sx, double *sy );
int  processCellsOnPrimAdjFace( Point2D cellPos2D, CELL *c,
							   int adjFaceIndex, int adjEdgeIndex,
							   double *sx, double *sy );
void insertFacesDistZeroInHeap( int whichFace );
int  compForcesOnAdjFaces( Point2D cellPos2D, CELL *c,
						  double *sx, double *sy );
void compForcesForCell( CELL *c, double *sumdx, double *sumdy );
int  includeCellsFromSecFaces( int whichFace, SCELL *h, SCELL *t );
double 	distBetweenTri( int face1, int face2);
double 	distBetweenPointAndTri( Point3D p, int whichTriangle );
void 	insertFacesAdjToVertexInHeap( int referenceFace, int faceIndex,
									 int vertIndex );
